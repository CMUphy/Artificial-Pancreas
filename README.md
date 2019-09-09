# Artificial-Pancreas
## Develop in Linux Environment using Vscode  
[Remote WSL](https://code.visualstudio.com/docs/remote/wsl)  
## TCP/IP  
### Compile and Run  
1.Change directory to `PID` and run the command `g++ glucose.cpp -lpthread -lm -o test`  
2.Then run the command `./test` in the same directory  
3.Change directory to `Client` and run the command `g++ main.cpp -lpthread -lm -o test`  
4.Then run the command `./test` in the same directory  
Example:  
  
![image](https://github.com/pyaixy/Artificial-Pancreas/blob/master/picture/1.PNG)  
![image](https://github.com/pyaixy/Artificial-Pancreas/blob/master/picture/2.PNG) 
### Command for Doctor(Client) 
#### 1.Get_Data  
Input command `Get_Data`, then the server will report the last 30 data to doctor.  
![image](https://github.com/pyaixy/Artificial-Pancreas/blob/master/picture/3.PNG) 
#### 2.Change_PID  
Input command `Change_PID`, then the server will return `Cin Paramenter Kc,tauI,tauD;`  
Input the value you want to change, end with enter  
After the server change the paramenter, it will return a messeage `Send Successfully!`  
![image](https://github.com/pyaixy/Artificial-Pancreas/blob/master/picture/4.PNG) 
#### 3.Change_setpoint  
Input command `Get_Data`, then the server will return `Set Model---1 for setpoint /2 for set range;`  
Select the model you want PID to work, then the server will return `Cin Setpoint` or `Cin UperLimits and LowerLimits`  
Input the value you want to set, end with enter  
After the server change the setpoint, it will return a messeage `Send Successfully!`  
![image](https://github.com/pyaixy/Artificial-Pancreas/blob/master/picture/5.PNG) 
![image](https://github.com/pyaixy/Artificial-Pancreas/blob/master/picture/6.PNG) 
#### 4.quit
Input command `quit`, then the client will exit, and the server will wait for the next connection  
![image](https://github.com/pyaixy/Artificial-Pancreas/blob/master/picture/7.PNG) 
