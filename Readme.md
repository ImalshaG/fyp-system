# Files

- B_AT - gets interrupted by central uC
- C_uno - sends serial data
- C_node - reads serial port and send to computer. Can use two // serial ports
- D_AT - prints "uint8"+"e"
- D_node - reads serial port till "e" and send to computer with inverted sawtooth
- E - Testing serial buffer of nodeMCU
- E_UNO - sends serial data to an ideling MCU and trigger it using pin 13
- E_node - wait till uno triggers it and read the serial buffer. max size = 257 bytes
- F_node - making the data buffer to send to server
- F_AT - sending serial data periodically
- G_node - sending data from NodeMCU and ATTiny to mongoDB
- H_node - data sending from ESP32 to mongoDB
- I_node - ESP32 two serial ports
- J_node - data sending to server from ESP32+serial ports
- K_node - lossless communication
- L_node - testing with arrays in JSON
- M_AT - I2C and Serial together
- N_328 - serial communication for 328p
- N_node - Transmitting based on interview
- O_node - ECG
- O_328 - simulated PAT
- P_328 - temeperature
- Q_328 - SpO2 + Bioz
- R_328 - Two Temp Sensors
