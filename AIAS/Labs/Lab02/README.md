- git clone error, ssh setting
    - [ssh setting](https://docs.gitlab.com/user/ssh/#rsa-ssh-keys)
    - After generate key, need to do this
```
eval $(ssh-agent -s)
ssh-add <directory to private SSH key>
```

- git remote
    - `git remote add gitlab ssh://git@course.playlab.tw:30022/X1136010/lab02.git`
    - X1136010 is ladp name

- Do lab using jupyter
```
## Start the Jupyter Notebook Server at default port 8888
$ run-jupyter

## You need to open your browser and go to the following
## URL: http://localhost:8888. 
## You might need to type the following command to get the token
$ jupyter server list

## stop jupyter notebook server
$ jupyter server stop 8888
```

- When done 
```
## the first time
$  git push --set-upstream gitlab main
## after the first time
$  git fetch origin main
## remember to solve conflicts
$  git merge origin/main
## then push back to your own repo
$  git push gitlab main
```

# Lab2-1

- **model** (`nn.module`): The neural network model.
- **input_size** (`Sequence of Sizes`): The size of the input tensor.
- **input_data** (`Sequence of Tensors`): Actual data to pass through the model (optional).
- **batch_dim** (`int`): The dimension used as the batch size.
- **cache_forward_pass** (`bool`): Whether to cache the forward pass.
- **col_names** (`Iterable[str]`): Column names for the summary output.
- **col_width** (`int`): The width of each column in the output.
- **depth** (`int`): Depth for nested summary.
- **device** (`torch.Device`): The device to run the model on.
- **dtypes** (`List[torch.dtype]`): Data types of the input tensor.
- **mode** (`str`): Mode for the summary.
- **row_settings** (`Iterable[str]`): Additional row settings.
- **verbose** (`int`): Verbosity level of the summary output.
- **\*\*kwargs**: Additional keyword arguments.

- `kill $(ps -e | grep 'tensorboard' | awk '{print $1}')` : Terminate TensorBoard

# lab2-4
- `sudo pip3 install cmake`
- [libtorch](https://pytorch.org/get-started/locally/)
```
## go to the working folder
$ cd ~/projects/lab02/lab2-4/analyzer

## download the library 
## https://download.pytorch.org/libtorch/cpu


## X86_64_LINK
$ wget https://download.pytorch.org/libtorch/cpu/libtorch-cxx11-abi-shared-with-deps-2.2.0%2Bcpu.zip

## unzip the file
$ unzip libtorch-cxx11-abi-shared-with-deps-2.2.0+cpu.zip 


## macOS arm64
$ wget https://file.playlab.tw/libtorch-aarch64-2.2.0+cpu.zip
$ unzip libtorch-aarch64-2.2.0+cpu.zip
```
- build and compile
```
## create build folder
mkdir ~/projects/lab02/lab2-4/analyzer/build

## enter the build folder
cd ~/projects/lab02/lab2-4/analyzer/build

## create config
cmake -DCMAKE_PREFIX_PATH=./libtorch ..

## compile
cmake --build . --config Release -j ${nproc}
```
- 
```
cd build
./analyzer ../../traced_resnet18.pt 
```

## HW2-3
- Deepseek, 過程中的 ERROR 就直接貼上，直到測試成功， 太猛了
- Promt 1
```
我要將 alexnet.onnx 中的 Gemm layers 替換成多個小的矩陣最後再 concat 起來，最後驗證的時候輸出的結果必須與原來的模型輸出的結果是一致的。舉例來說第一個 Gemm layer， A 是上一層 Flatten 的輸入 shape 為 [10, 9216], 這一層的 B (Weights) 為 [4096,9216] 但是 transB == 1 所以在運算時是 [9216,4096]， C(bias) 為 [4096] 會自動對應到輸出shape 為 [10,4096]。 我要將 [10,9216] 與 [9216,4096] 的矩陣運算 換成， A 共拆成 9 個 [10,1024], B 共拆成 9個rows， 4 個columns 的 [1024,1024] 的小矩陣來運算， 然後用 4 個 Sum [10,1024] 在對應的位置加總起來， 最後 將 Sum 的結果concat 起來 變回 [10,4096]。 如果有 bias 存在那麼請一併處理
```
- Promt 2
```
我需要自動偵測到 Gemm layers 然後將對應的shape 去切成 最大的 block 為 1024， 請依照上面的邏輯給我完整的程式碼
```
- Promt 3
```
Write a script to verify that the transformed AlexNet model is mathematically equivalent to the original AlexNet model. The verification should be performed using 100 random input samples. For each input, compare the outputs of the original and transformed models using np.allclose with atol=1e-5. Ensure that all 100 input samples satisfy the condition for correctness.
請根據要求來驗證
```