import os

os.environ['XLA_FLAGS'] = '--xla_gpu_cuda_data_dir=/usr/lib/nvidia-cuda-toolkit'

import tensorflow as tf

import numpy as np
from tensorflow.keras import layers, models, callbacks, optimizers
from tensorflow.keras.callbacks import CSVLogger
from trabalhos.trabalhos.tp2_sistemas_paralelos.cnn.utils_cnn import load_labels_from_dir, CHAR_TO_IDX, NUM_CLASSES

# ————— Caminhos —————
HERE       = os.path.dirname(os.path.abspath(__file__))
PROJECT    = os.path.dirname(HERE)
BASE_DIR   = os.path.join(PROJECT, "TP2_versao2/CAPTCHA-10k")
LABELS_DIR = os.path.join(BASE_DIR, "labels10k")
TRAIN_DIR  = os.path.join(BASE_DIR, "treinamento")
VAL_DIR    = os.path.join(BASE_DIR, "validacao")

for path in (LABELS_DIR, TRAIN_DIR, VAL_DIR):
    if not os.path.isdir(path):
        raise FileNotFoundError(f"Pasta não encontrada: {path}")

# ————— Hiper-parâmetros —————
BATCH_SIZE = 32
IMG_HEIGHT = 50
IMG_WIDTH  = 180
EPOCHS     = 40
LR         = 1e-2

# ————— Carrega rótulos —————
labels = load_labels_from_dir(LABELS_DIR)

def one_hot_label(lbl):
    arr = np.zeros((6, NUM_CLASSES), dtype=np.float32)
    for i, ch in enumerate(lbl):
        arr[i, CHAR_TO_IDX[ch]] = 1.0
    return arr

# ————— Camada de tradução —————
translation_layer = layers.RandomTranslation(
    height_factor=2/IMG_HEIGHT,
    width_factor=2/IMG_WIDTH,
    fill_mode='reflect'
)

# ————— Preprocess + augment —————
def preprocess_and_augment(path, y_arr, train=True):
    img = tf.io.read_file(path)
    img = tf.image.decode_png(img, channels=1)
    img = tf.image.resize(img, [IMG_HEIGHT, IMG_WIDTH]) / 255.0
    if train:
        img = tf.image.random_brightness(img, 0.2)
        img = tf.image.random_contrast(img, 0.8, 1.2)
        img = translation_layer(img)
    # separa o vetor one-hot em 6 vetores e monta dict com as chaves das camadas de saída
    heads = tf.unstack(y_arr, axis=0)
    y_dict = { f"char{i+1}": heads[i] for i in range(6) }
    return img, y_dict

# ————— DataPipeline —————
def make_dataset(folder, train=False):
    files, y_arrays = [], []
    for fname in os.listdir(folder):
        if fname.lower().endswith(('.png','.jpg','.jpeg')):
            base = os.path.splitext(fname)[0]
            if base in labels:
                files.append(os.path.join(folder, fname))
                y_arrays.append(one_hot_label(labels[base]))
    ds = tf.data.Dataset.from_tensor_slices((files, y_arrays))
    if train:
        ds = ds.shuffle(len(files))
    ds = ds.map(lambda p, y: preprocess_and_augment(p, y, train),
                num_parallel_calls=tf.data.AUTOTUNE)
    return ds.batch(BATCH_SIZE).prefetch(tf.data.AUTOTUNE)

train_ds = make_dataset(TRAIN_DIR, train=True)
val_ds   = make_dataset(VAL_DIR,   train=False)

# ————— Monta a CNN —————
inputs = layers.Input(shape=(IMG_HEIGHT, IMG_WIDTH, 1))
x = inputs
for filters in (64, 128, 256):
    x = layers.Conv2D(filters, 3, padding='same')(x)
    x = layers.BatchNormalization()(x)
    x = layers.LeakyReLU()(x)
    x = layers.MaxPool2D()(x)
x = layers.Flatten()(x)
x = layers.Dense(256)(x)
x = layers.LeakyReLU()(x)
x = layers.Dropout(0.5)(x)

# 6 saídas softmax nomeadas
outs = [
    layers.Dense(NUM_CLASSES, activation='softmax', name=f"char{i+1}")(x)
    for i in range(6)
]
model = models.Model(inputs=inputs, outputs=outs)

# ————— Compilação —————
losses  = {f"char{i+1}": "categorical_crossentropy" for i in range(6)}
metrics = {f"char{i+1}": "accuracy"               for i in range(6)}
opt     = optimizers.Adam(learning_rate=LR)
model.compile(optimizer=opt, loss=losses, metrics=metrics)

# ————— Callbacks incluindo CSVLogger —————
csv_logger = CSVLogger('training_log.csv', append=False)
cbks = [
    callbacks.ReduceLROnPlateau(monitor='val_loss', factor=0.5, patience=3),
    callbacks.EarlyStopping(monitor='val_loss', patience=5, restore_best_weights=True),
    csv_logger
]

# ————— Treino —————
history = model.fit(
    train_ds,
    validation_data=val_ds,
    epochs=EPOCHS,
    callbacks=cbks
)

# ————— Salva modelo e imprime métricas finais —————
model.save('cnn_captcha.keras')
print('Modelo salvo em cnn_captcha.keras')
for i in range(6):
    ta = history.history[f'char{i+1}_accuracy'][-1]
    va = history.history[f'val_char{i+1}_accuracy'][-1]
    print(f"char{i+1} — train_acc={ta:.4f}, val_acc={va:.4f}")
