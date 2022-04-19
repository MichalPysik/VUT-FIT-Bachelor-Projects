import tensorflow as tf
from tensorflow import keras
from tensorflow.keras import layers
import os
import matplotlib.pyplot as plt



#GENERATE DATASET
image_size = (180, 180)
batch_size = 32

train_ds = tf.keras.preprocessing.image_dataset_from_directory(
    "PetImages", validation_split=0.2,
    subset="training", seed=1337,
    image_size=image_size, batch_size=batch_size,
)
val_ds = tf.keras.preprocessing.image_dataset_from_directory(
    "PetImages", validation_split=0.2,
    subset="validation", seed=1337,
    image_size=image_size, batch_size=batch_size,
)


#VISUALIZING THE IMAGE DATA
#plt.figure(figsize=(10, 10))
#for images, labels in train_ds.take(1):
#    for i in range(9):
#        ax = plt.subplot(3, 3, i + 1)
#        plt.imshow(images[i].numpy().astype("uint8"))
#        plt.title(int(labels[i]))
#        plt.axis("off")
#plt.show()



#USING IMAGE DATA AUGMENTATION
data_augmentation = keras.Sequential(
    [
        layers.RandomFlip("horizontal"),
        layers.RandomRotation(0.1),
    ]
)
#visualize the new transformed images
#plt.figure(figsize=(10, 10))
#for images, labels in train_ds.take(1):
#    for i in range(9):
#        augmented_images = data_augmentation(images)
#        ax = plt.subplot(3, 3, i + 1)
#        plt.imshow(augmented_images[0].numpy().astype("uint8"))
#        plt.axis("off")
#plt.show()




#STANDARDIZING DATA (option 2) - Applying it to the dataset (CPU only)
#augmented_train_ds = train_ds.map(
#    lambda x, y: (data_augmentation(x, training=True), y))



#CONFIGURE DATASET FOR PERFORMANCE - buffered prefetching
train_ds = train_ds.prefetch(buffer_size=32)
val_ds = val_ds.prefetch(buffer_size=32)





#BUILDING A MODEL (small version of the Xception network)
def make_model(input_shape, num_classes):
    inputs = keras.Input(shape=input_shape)
    #Image augmentation block
    x = data_augmentation(inputs)

    #Entry block
    x = layers.Rescaling(1.0 / 255)(x)
    x = layers.Conv2D(32, 3, strides=2, padding="same")(x)
    x = layers.BatchNormalization()(x)
    x = layers.Activation("relu")(x)

    x = layers.Conv2D(64, 3, padding="same")(x)
    x = layers.BatchNormalization()(x)
    x = layers.Activation("relu")(x)

    previous_block_activation = x #Set aside residual

    for size in [128, 256, 512, 728]:
        x = layers.Activation("relu")(x)
        x = layers.SeparableConv2D(size, 3, padding="same")(x)
        x = layers.BatchNormalization()(x)

        x = layers.Activation("relu")(x)
        x = layers.SeparableConv2D(size, 3, padding="same")(x)
        x = layers.BatchNormalization()(x)

        x = layers.MaxPooling2D(3, strides=2, padding="same")(x)

        #Project residual
        residual = layers.Conv2D(size, 1, strides=2, padding="same")(
            previous_block_activation
        )
        x = layers.add([x, residual]) #Add back residual
        previous_block_activation = x #Set aside next residual

    x = layers.SeparableConv2D(1024, 3, padding="same")(x)
    x = layers.BatchNormalization()(x)
    x = layers.Activation("relu")(x)

    x = layers.GlobalAveragePooling2D()(x)
    if num_classes == 2:
        activation = "sigmoid"
        units = 1
    else:
        activation = "softmax"
        units = num_classes

    x = layers.Dropout(0.5)(x)
    outputs = layers.Dense(units, activation=activation)(x)
    return keras.Model(inputs, outputs)


model = make_model(input_shape=image_size + (3,), num_classes=2)

#keras.utils.plot_model(model, show_shapes=True)





#TRAINING THE MODEL
epochs = 25

callbacks = [
    keras.callbacks.ModelCheckpoint("save_at_{epoch}.h5"),
]
model.compile(
    optimizer=keras.optimizers.Adam(1e-3),
    loss="binary_crossentropy",
    metrics=["accuracy"],
)

model.load_weights("latest_save.h5") # load save before training

#model.fit(
#    train_ds, epochs=epochs, callbacks=callbacks, validation_data=val_ds,
#) #train






#INFERENCE ON NEW DATA
testdir = "test_images"
for image_name in sorted( os.listdir(testdir) ):
    image_name = testdir + "/" + image_name
    img = keras.preprocessing.image.load_img( image_name, target_size=image_size )
    img_array = keras.preprocessing.image.img_to_array(img)
    img_array = tf.expand_dims(img_array, 0) #Create both axis

    predictions = model.predict(img_array)
    score = predictions[0]
    print( "\n%s is %.2f percent cat and %.2f percent dog." % (image_name, 100 * (1 - score), 100 * score) )

    if image_name == 'test_images/endy3.jpg':
        plt.imshow(plt.imread(image_name))
        plt.axis('off')
        plt.text(50, 2600, "endy3.jpg is 0.05 percent cat and 99.95 percent dog.", backgroundcolor='#ffffffc0', size=13)
        plt.show()
    elif image_name == 'test_images/playing-cat.jpg':
        plt.imshow(plt.imread(image_name))
        plt.axis('off')
        plt.text(5, 226, "playing-cat.jpg is 94.68 percent cat and 5.32 percent dog.", backgroundcolor='#ffffffc0', size=12)
        plt.show()
