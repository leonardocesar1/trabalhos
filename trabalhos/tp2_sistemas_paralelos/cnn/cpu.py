import tensorflow as tf
from tensorflow import keras
from tensorflow.keras import layers
import numpy as np
import multiprocessing


def build_model():
    model = keras.Sequential([
        keras.Input(shape=(28, 28, 1)),
        layers.Conv2D(32, kernel_size=(3, 3), activation="relu"),
        layers.MaxPooling2D(pool_size=(2, 2)),
        layers.Flatten(),
        layers.Dense(10, activation="softmax")
    ])
    return model


optimizer = tf.keras.optimizers.Adam()
loss_fn = tf.keras.losses.SparseCategoricalCrossentropy()


def worker_process_func(args):
    model_weights, sub_batch_data, sub_batch_labels = args

    worker_model = build_model()

    worker_model.set_weights(model_weights)

    with tf.GradientTape() as tape:
        predictions = worker_model(sub_batch_data, training=True)
        loss = loss_fn(sub_batch_labels, predictions)

    gradients = tape.gradient(loss, worker_model.trainable_variables)

    return gradients


if __name__ == "__main__":
    NUM_WORKERS = 4  # pode altera para o tanto de núcleos

    print(
        f"Iniciando treinamento paralelo manual em {NUM_WORKERS} processos CPU...")

    master_model = build_model()

    (x_train, y_train), _ = tf.keras.datasets.mnist.load_data()
    x_train = np.expand_dims(x_train, -1).astype("float32") / 255.0

    EPOCHS = 3
    BATCH_SIZE = 64

    pool = multiprocessing.Pool(processes=NUM_WORKERS)

    for epoch in range(EPOCHS):
        print(f"Iniciando Epoch {epoch+1}/{EPOCHS}")

        for step in range(len(x_train) // BATCH_SIZE):
            start = step * BATCH_SIZE
            end = (step + 1) * BATCH_SIZE
            batch_data = x_train[start:end]
            batch_labels = y_train[start:end]

            current_weights = master_model.get_weights()

            data_splits = np.array_split(batch_data, NUM_WORKERS)
            label_splits = np.array_split(batch_labels, NUM_WORKERS)

            task_args = [
                (current_weights, data_splits[i], label_splits[i])
                for i in range(NUM_WORKERS)
            ]

            list_of_gradients = pool.map(worker_process_func, task_args)

            aggregated_gradients = []
            for gradients_per_layer in zip(*list_of_gradients):
                mean_gradient = tf.add_n(gradients_per_layer) / NUM_WORKERS
                aggregated_gradients.append(mean_gradient)

            optimizer.apply_gradients(
                zip(aggregated_gradients, master_model.trainable_variables))

            if step % 20 == 0:
                preds = master_model(batch_data, training=False)
                loss_val = loss_fn(batch_labels, preds)
                print(f"  Step {step}: loss = {loss_val.numpy():.4f}")

    pool.close()
    pool.join()
