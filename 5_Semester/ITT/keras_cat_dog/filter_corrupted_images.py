import os
import tensorflow as tf


images_deleted = 0
dogs_deleted = 0
cats_deleted = 0

for folder_name in ("Cat", "Dog"):
    folder_path = os.path.join("PetImages", folder_name)
    for fname in os.listdir(folder_path):
        fpath = os.path.join(folder_path, fname)
        try:
            fobj = open(fpath, "rb")
            is_jfif = tf.compat.as_bytes("JFIF") in fobj.peek(10)
        finally:
            fobj.close()

        if not is_jfif:
            images_deleted += 1
            if folder_name == "Dog":
                dogs_deleted += 1
            else:
                cats_deleted += 1
            #image is corrupted, delete
            os.remove(fpath)


print("Successfully removed ", images_deleted, " images")
print("Which contained ", dogs_deleted, " dog images")
print("And ", cats_deleted, " cat images")
