from google.protobuf import descriptor as _descriptor
from google.protobuf import message as _message
from typing import ClassVar as _ClassVar, Optional as _Optional

DESCRIPTOR: _descriptor.FileDescriptor

class simpleUploadRequest(_message.Message):
    __slots__ = ("clientCalculatedHash", "fileData")
    CLIENTCALCULATEDHASH_FIELD_NUMBER: _ClassVar[int]
    FILEDATA_FIELD_NUMBER: _ClassVar[int]
    clientCalculatedHash: str
    fileData: bytes
    def __init__(self, clientCalculatedHash: _Optional[str] = ..., fileData: _Optional[bytes] = ...) -> None: ...

class simpleUploadReply(_message.Message):
    __slots__ = ("statusCode", "detail")
    STATUSCODE_FIELD_NUMBER: _ClassVar[int]
    DETAIL_FIELD_NUMBER: _ClassVar[int]
    statusCode: int
    detail: str
    def __init__(self, statusCode: _Optional[int] = ..., detail: _Optional[str] = ...) -> None: ...
