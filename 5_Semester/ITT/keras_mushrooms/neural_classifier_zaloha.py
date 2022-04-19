# Hluboka neuronova sit pro klasifikaci jedlych/jedovatych hub
# https://machinelearningmastery.com/tutorial-first-neural-network-python-keras/
# https://www.kaggle.com/claudiav/mushroom-classifier
from keras.models import Sequential
from keras.layers import Dense, Dropout
import pandas as pd
from sklearn import preprocessing
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


# nacist dataset z csv souboru
dataset = pd.read_csv("agaricus-lepiota.csv", header=None, delimiter=',', names=column_names)

dataset.drop(['stalk-root'], axis=1, inplace=True) #tento atribut casto chybi, vypustime ho

dataset = dataset.sample(frac=1).reset_index(drop=True) #nahodne promichat dataset a resetovat indexovani od 0

one_hot_enc = preprocessing.OneHotEncoder(sparse=False) #transformovat na one hot encoding, at je jasne ze zde neni hiearchie
dataset = one_hot_enc.fit_transform(dataset)
print(dataset.shape)


col_count = dataset.shape[1] #pocet sloupcu, prvni je trida
row_count = dataset.shape[0] #pocet radku datasetu
split_point = int(row_count * 0.7) #misto kde rozdelime dataset na trenink a validaci


X_train = dataset[:split_point, 2:col_count] # Zbyvajici sloupce jsou dalsi atributy
y_train = dataset[:split_point, 0:1] # Prvni sloupec je nas vystup -> trida (jedla, jedovata)

X_validate = dataset[split_point:, 2:col_count] # Zbyvajici sloupce jsou dalsi atributy
y_validate = dataset[split_point:, 0:1] # Prvni sloupec je nas vystup -> trida (jedla, jedovata)


# model se 2 vrstvami
model = Sequential()
model.add(Dense(16, input_dim=col_count-2, activation="relu")) # vstupni vrstva
model.add(Dropout(0.5)) #dropout pro zamezeni overfittingu
model.add(Dense(1, activation="sigmoid")) # vystupni vrstva

# kompilace modelu a dalsi parametry
model.compile(loss="binary_crossentropy", optimizer="adam", metrics=["accuracy"])
model.fit(X_train, y_train, epochs=2, batch_size=32)

_, accuracy = model.evaluate(X_train, y_train)
print('Accuracy train percentage: %.2f' % (accuracy*100))

_, accuracy = model.evaluate(X_validate, y_validate)
print('Accuracy validate percentage: %.2f' % (accuracy*100))

VYSLEDKY = model.predict(X_validate)
print(VYSLEDKY)
print(y_validate)