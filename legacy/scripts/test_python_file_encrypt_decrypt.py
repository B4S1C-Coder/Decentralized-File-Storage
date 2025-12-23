import os
import ctypes
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent

LIB_PATH = os.path.join(ROOT, "build", "dev", "dfs-core-dev-linux-with-python.so")
    
lib = ctypes.CDLL(LIB_PATH)

class keyNoncePair_secretbox(ctypes.Structure):
    _fields_ = [
        ("statusCode", ctypes.c_int),
        ("key", ctypes.c_ubyte * 32),
        ("nonce", ctypes.c_ubyte * 24)
    ]

lib.perform_dfs_divideFileIntoChunks_encryptedSecretbox.argtypes = [
    ctypes.c_char_p, ctypes.c_char_p, ctypes.c_int,
    ctypes.c_char_p, ctypes.c_int, ctypes.c_char_p
]

lib.perform_dfs_divideFileIntoChunks_encryptedSecretbox.restype = keyNoncePair_secretbox

lib.perform_dfs_decryptAndReconstructFile_secretbox.argtypes = [
    ctypes.POINTER(keyNoncePair_secretbox), ctypes.c_char_p, ctypes.c_int,
    ctypes.c_int, ctypes.POINTER(ctypes.c_char_p)
]

print("Lib loaded successfully")

# Encrypt and split file into chunks
input_file_path = os.path.join(ROOT, "debug", "text1.txt").encode('utf-8')
input_file_name = b"text1.txt"
num_chunks = 5
header_token = b"<OWNERSHIP-TOKEN>"
header_size_in_bytes = len("<OWNERSHIP-TOKEN>")
output_dir_path = os.path.join(ROOT, "debug", "chunks", "").encode('utf-8')

result = lib.perform_dfs_divideFileIntoChunks_encryptedSecretbox(
    input_file_path, input_file_name, num_chunks, header_token,
    header_size_in_bytes, output_dir_path
)

print(f"Encryption exit status: {result.statusCode}")

if result.statusCode < 0:
    print("Encryption failed.")
    exit()

file_paths = []

for root, directories, files in os.walk(os.path.join(ROOT, "debug", "chunks")):
    for filename in files:
        file_paths.append(os.path.join(root, filename).encode('utf-8'))

output_file_path = os.path.join(ROOT, "debug", "decrypted.txt").encode('utf-8')
chunk_file_paths = (ctypes.c_char_p * num_chunks)(*file_paths)

status = lib.perform_dfs_decryptAndReconstructFile_secretbox(
    ctypes.byref(result), output_file_path, header_size_in_bytes, num_chunks, chunk_file_paths
)

print(f"Decryption exit status: {status}")
print("Script completed.")
