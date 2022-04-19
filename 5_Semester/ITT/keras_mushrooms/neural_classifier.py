# Hluboka neuronova sit pro klasifikaci jedlych/jedovatych hub
# https://machinelearningmastery.com/tutorial-first-neural-network-python-keras/
# https://www.kaggle.com/claudiav/mushroom-classifier
from keras.models import Sequential
from keras.layers import Dense, Dropout
import pandas as pd
from sklearn.preprocessing import OneHotEncoder, LabelEncoder
from sklearn.model_selection import train_test_split
import numpy as np

# nazvy jedlotlivych sloupcu v datasetu, prvnim je trida
column_names = ['class',
                'cap-shape',
                'cap-surface',
                'cap-color',
                'bruises',
                'odor',
                'gill-attachment',
                'gill-spacing',
                'gill-size',
                'gill-color',
                'stalk-shape',
                'stalk-root',
                'stalk-surface-above-ring',
                'stalk-surface-below-ring',
                'stalk-color-above-ring',
                'stalk-color-below-ring',
                'veil-type',
                'veil-color',
                'ring-number',
                'ring-type',
                'spore-print-color',
                'population',
                'habitat']

dataset = pd.read_csv('./agaricus-lepiota.csv')
dataset.drop(['stalk-root'], axis=1, inplace=True) #tento atribut casto chybi, vypustime ho

X = dataset.iloc[:, 1:23].values
y = dataset.iloc[:, 0].values

le = LabelEncoder()
le.fit(['p','e'])
y = le.transform(y)

ohe = OneHotEncoder(sparse=False)
X = ohe.fit_transform(X)

X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.2, stratify=y, random_state=23)

# model se 3 vrstvami (1 skryta)
model = Sequential()
model.add(Dense(16, input_dim=X.shape[1], activation="relu")) # vstupni vrstva
model.add(Dense(8, activation="relu")) # skryta vrstva
model.add(Dense(1, activation="sigmoid")) # vystupni vrstva

model.compile(loss="binary_crossentropy", optimizer="adam", metrics=["accuracy"])
model.fit(X_train, y_train, epochs=10, batch_size=32)

_, accuracy = model.evaluate(X_test, y_test)
print('Test accurracy percentage: %.2f' % (accuracy*100))
