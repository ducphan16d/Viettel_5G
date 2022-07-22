#Viettel_5G_COURSE
# Implement
```
git clone https://github.com/ducphan16d/Viettel_5G.git
```
Change directory to the Viettel_5G_course directory that just clone
```
cd Viettel_5G
```
### Compile files
```
gcc -o gNB gNB_server.c -lpthread
gcc -o AMF AMF_Client.c -lpthread
gcc -o UE UE_Client.c -lpthread 
```
### Run gNB_Server
```
./gNB
```
### Run UE_Client on another Terminal
```
./UE
```
### Run AMF_Client on another Terminal
```
./AMF
```