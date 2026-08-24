# evaluate_cnn_captcha.py
import os
import numpy as np
import tensorflow as tf
import cv2
import matplotlib.pyplot as plt
import seaborn as sns
from sklearn.metrics import confusion_matrix, ConfusionMatrixDisplay

os.environ['XLA_FLAGS'] = '--xla_gpu_cuda_data_dir=/usr/lib/nvidia-cuda-toolkit'

from trabalhos.trabalhos.tp2_sistemas_paralelos.cnn.utils_cnn import load_labels_from_dir, CHAR_TO_IDX, NUM_CLASSES

# Caminhos
HERE       = os.path.dirname(os.path.abspath(__file__))
BASE_DIR   = os.path.join(HERE, "CAPTCHA-10k")
LABELS_DIR = os.path.join(BASE_DIR, "labels10k")
TEST_DIR   = os.path.join(BASE_DIR, "teste")
RESULT_DIR = os.path.join(HERE, "results_cnn")
os.makedirs(RESULT_DIR, exist_ok=True)

# Carrega labels e modelo
labels = load_labels_from_dir(LABELS_DIR)
model_path = os.path.join(HERE, "cnn_captcha.keras")
model = tf.keras.models.load_model(model_path)

# Tamanho esperado pelo modelo
_, IMG_HEIGHT, IMG_WIDTH, _ = model.input_shape
IDX_TO_CHAR = {v: k for k, v in CHAR_TO_IDX.items()}

# Pré-processamento
def preprocess(path):
    img = tf.io.read_file(path)
    img = tf.image.decode_png(img, channels=1)
    img = tf.image.resize(img, [IMG_HEIGHT, IMG_WIDTH]) / 255.0
    return img.numpy()

# Coleta arquivos e dados
file_names = sorted([
    f for f in os.listdir(TEST_DIR)
    if f.lower().endswith(('.png', '.jpg', '.jpeg'))
    and os.path.splitext(f)[0] in labels
])

X_list, gt_labels, paths = [], [], []

for fname in file_names:
    base = os.path.splitext(fname)[0]
    gt_labels.append(labels[base])
    paths.append(os.path.join(TEST_DIR, fname))
    X_list.append(preprocess(paths[-1]))

X_test = np.stack(X_list)
y_true_strs = gt_labels

# Predição
preds = model.predict(X_test, batch_size=32, verbose=1)
pred_labels, correct_counts = [], []
all_y_true, all_y_pred = [], [[] for _ in range(6)]  # por caractere

for i in range(len(X_test)):
    pred_chars = [IDX_TO_CHAR[np.argmax(preds[j][i])] for j in range(6)]
    true_chars = list(gt_labels[i])
    pred_str = ''.join(pred_chars)

    pred_labels.append(pred_str)
    correct = sum([p == t for p, t in zip(pred_str, gt_labels[i])])
    correct_counts.append(correct)

    # Por caractere
    for j in range(6):
        all_y_true.append(true_chars[j])
        all_y_pred[j].append(pred_chars[j])

    # Imagem anotada
    img = cv2.imread(paths[i])
    cv2.putText(img, pred_str, (5, 25), cv2.FONT_HERSHEY_SIMPLEX, 0.8, (0, 0, 255), 2)
    cv2.imwrite(os.path.join(RESULT_DIR, os.path.basename(paths[i])), img)

# Acurácias
total = len(correct_counts)
rates = [sum(c >= k for c in correct_counts) / total for k in range(7)]
char_acc = [np.mean([t == p for t, p in zip(gt_labels, pred_labels)]) for _ in range(6)]
str_acc = rates[6]

print(f"Acurácia de string completa: {str_acc:.4f}")
for i in range(6):
    pos_acc = np.mean([true[i] == pred[i] for true, pred in zip(gt_labels, pred_labels)])
    print(f"Acurácia posição {i+1}: {pos_acc:.4f}")

# ————— Gráficos —————

# 1. Taxa de reconhecimento por caracteres corretos
plt.figure()
plt.plot(range(7), rates, marker='o', color='darkorange')
plt.ylim(0, 1.02)
plt.xlabel('Mínimo de caracteres corretos')
plt.ylabel('Taxa de reconhecimento')
plt.title('CNN — Avaliação por acerto mínimo')
plt.grid(True)
plt.tight_layout()
plt.savefig('cnn_recognition_rate.png', dpi=300)

# 2. Acurácia por posição
plt.figure()
positions = [f"char{i+1}" for i in range(6)]
accs = [np.mean([t[i] == p[i] for t, p in zip(gt_labels, pred_labels)]) for i in range(6)]
plt.bar(positions, accs, color='steelblue')
plt.ylim(0, 1.02)
plt.ylabel('Acurácia')
plt.title('Acurácia por posição do caractere')
plt.tight_layout()
plt.savefig('cnn_positional_accuracy.png', dpi=300)

# 3. Matriz de confusão (geral)
y_true_flat = [c for lbl in gt_labels for c in lbl]
y_pred_flat = [c for lbl in pred_labels for c in lbl]
cm = confusion_matrix(y_true_flat, y_pred_flat, labels=list(CHAR_TO_IDX.keys()))
plt.figure(figsize=(10, 9))
sns.heatmap(cm, xticklabels=CHAR_TO_IDX.keys(), yticklabels=CHAR_TO_IDX.keys(),
            cmap='Blues', fmt='d', square=True)
plt.xlabel("Predito")
plt.ylabel("Verdadeiro")
plt.title("Matriz de confusão geral")
plt.tight_layout()
plt.savefig("cnn_confusion_matrix.png", dpi=300)

print("Imagens salvas em 'results_cnn/' e gráficos no diretório atual.")
