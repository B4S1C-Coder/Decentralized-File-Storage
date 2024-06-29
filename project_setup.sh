#!/usr/bin/bash
echo "------------------- [ Decentralized File Storage ] -------------------"
echo "Time:" `date`
echo "Platform:" `uname`

echo -e "\nPlease ensure you have the following commands working before proceeding: "
echo "1) python3"
echo "2) g++"
echo "3) git"
echo "4) virtualenv"
echo -e -n "\nAre these working? [y: proceed, any other key to exit] > "
read user_choice

if [ "$user_choice" != "y" ]; then
    exit
fi

echo -e -n "Cloning repo: https://github.com/B4S1C-Coder/Decentralized-File-Storage.git ..."
git clone https://github.com/B4S1C-Coder/Decentralized-File-Storage.git

cd Decentralized-File-Storage

echo "Making directories and files for test setup ..."
mkdir dfs_user_client/client_storage/
mkdir dfs_user_client/client_storage/chunks/
mkdir dfs_user_client/client_storage/chunks/client_storage/
touch dfs_user_client/client_storage/text1.txt

echo "Making virtual environment (for python) ..."
virtualenv .proto-env
source ./.proto-env/bin/activate
pip3 install -r requirements.txt

echo "Re-building _pb2_grpc.py and _pb2.py as a precaution ..."
python3 -m grpc_tools.protoc -I"protos/" --python_out="dfs_storage_node/" --pyi_out="dfs_storage_node/" --grpc_python_out="dfs_storage_node/" protos/filetransfer.proto
cp dfs_storage_node/*_pb2.py dfs_user_client/
cp dfs_storage_node/*_pb2.pyi dfs_user_client/
cp dfs_storage_node/*_pb2_grpc.py dfs_user_client/

deactivate

echo -e -n "\n--------------------------\nIs this a debian based distro? [y: yes, any other key for no] > "
read is_debian_based

if [ "$is_debian_based" = "y" ]; then
    echo "For debian based systems lib sodium can be installed via apt package manager or built from source."
    echo -n "How would you like to proceed ? [1: apt way, 2: build from source, any other to abort] > "
    read sodium_build_choice

    if [ "$sodium_build_choice" = "1" ]; then
        echo "Installing libsodium-dev ..."
        sudo apt-get install libsodium-dev
        echo "Locating libsodium.so ..."
        sodium_path=$(find /usr/lib -name "libsodium.so" | head -n 1)
        sodium_path_a=$(find /usr/lib -name "libsodium.a" | head -n 1)
        
        if [ -z "$sodium_path" ]; then
            echo "libsodium.so was not found, if installation was unsuccessful consider building from source"
            exit
        fi

        if [ -z "$sodium_path_a" ]; then
            echo "libsodium.a was not found, if installation was unsuccessful consider building from source"
            exit
        fi

        echo "Located at: $sodium_path"
        echo "Copying to build directory ..."
        cp $sodium_path "./build/dev/libsodium.so"
        cp $sodium_path "./build/release/libsodium.so"

        echo "Located at: $sodium_path_a"
        echo "Copying to libs directory ..."
        cp $sodium_path "./libs/libsodium.a"

    elif [ "$sodium_build_choice" = "2" ]; then
        echo "Build sodium from source has currently not been automated."
        echo "Please visit: https://doc.libsodium.org/installation for building libsodium from source."
        echo "NOTE: By default it will produce a static library, please ensure that you build a dynamic library (.so or .dll)."

    fi

fi

echo "PLEASE POPULATE dfs_user_client/client_storage/text1.txt WITH DUMMY DATA IN ORDER TO RUN TESTS."
