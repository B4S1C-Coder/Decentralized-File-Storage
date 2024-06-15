import ctypes

lib = ctypes.CDLL(r"C:\Users\User\PrivateUnifiedCodebase\tests\cpp_file_io\dlls\fileops_tests.dll")

lib.copyContentsOfFile.argtypes = [ctypes.c_char_p, ctypes.c_char_p]
lib.copyContentsOfFile.restype = ctypes.c_int

lib.divideFileIntoChunks_unencrypted.argtypes = [
	ctypes.c_char_p, ctypes.c_int, ctypes.c_char_p,
	ctypes.c_int, ctypes.c_char_p,
]

lib.divideFileIntoChunks_unencrypted.restype = ctypes.c_int

# input_file = b"text1.txt"
# output_file = b"text6.txt"

# result = lib.copyContentsOfFile(input_file, output_file)
# print("Return status code:", result)

input_file = b"text1.txt"
num_chunks = 5
header_token = b"<OWNERSHIP-TOKEN>"
header_size_in_bytes = len(header_token)
output_dir_path = b"chunks_via_python2/"

result = lib.divideFileIntoChunks_unencrypted(input_file, num_chunks, header_token,
	header_size_in_bytes, output_dir_path)
print("Result status code:", result)