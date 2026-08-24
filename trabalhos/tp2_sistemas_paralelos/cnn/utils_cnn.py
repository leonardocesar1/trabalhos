import os

CHARS       = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"
CHAR_TO_IDX = {c: i for i, c in enumerate(CHARS)}
NUM_CLASSES = len(CHARS)

def load_labels_from_dir(labels_dir):
    """
    Varre todos os .txt em labels_dir e devolve dict:
       chave = base name (ex: "001384"), 
       valor = string-verdade de 6 chars (vinda do .txt).
    """
    d = {}
    for fname in os.listdir(labels_dir):
        # agora aceita .txt em vez de .csv
        if not fname.lower().endswith(".txt"):
            continue
        base = os.path.splitext(fname)[0]
        path = os.path.join(labels_dir, fname)
        with open(path, "r", encoding="utf-8") as f:
            label = f.read().strip().upper()
        # só guarda se for exatamente 6 chars e todos válidos
        if len(label) == 6 and all(ch in CHAR_TO_IDX for ch in label):
            d[base] = label
    return d
