import ctypes
from pathlib import Path
import os

ROOT_DIR = Path(__file__).resolve().parent.parent
so_dll_path = os.path.join(ROOT_DIR, "debug", r"1718453234-Linux-4.4.0-19041-Microsoft-x86_64-with-glibc2.35-dfs-core-v1.0.0.so")
lib = ctypes.CDLL(so_dll_path)

lib.divideFileIntoChunks_unencrypted.argtypes = [
	ctypes.c_char_p, ctypes.c_int, ctypes.c_char_p,
	ctypes.c_int, ctypes.c_char_p,
]

lib.divideFileIntoChunks_unencrypted.restype = ctypes.c_int

# This is a wrapper function on the c function
def divideFileIntoChunks_unencrypted(input_file: str, num_chunks: int, header_token: str,
	header_size_in_bytes: int, output_dir_path: str) -> int | None:

	print(f"[ DEBUG ] {input_file.encode()}")
	
	try:
		status_code = lib.divideFileIntoChunks_unencrypted(
			input_file.encode('utf-8'),
			num_chunks,
			header_token.encode('utf-8'),
			header_size_in_bytes,
			output_dir_path.encode('utf-8')
		)

		if status_code == -1:
			return None

		return status_code

	except Exception as err:
		return None