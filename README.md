# moos-ivp-velodyne

### Velodyne Drivers for MOOS-IvP
Implementation of VelodyneHDL (https://github.com/nicrip/VelodyneHDL) drivers for MOOS-IvP.

#### Dependencies
###### VelodyneHDL Library:
> (https://github.com/nicrip/VelodyneHDL)

###### MOOS-IvP V10:
> You must have MOOSV10 for these drivers to work correctly, with MOOS built with asynchronous communications support - Velodyne packets are streamed so quickly that MOOS pre-V10 cannot keep up

#### Build  

###### Building:
> ./build.sh

###### Add To .bashrc:
> export PATH="PATH:/..../path/to/bin

#### Usage  

###### Publishing Velodyne Packets to MOOSDB:
> iVelodyneHDL iVelodyneHDL.moos  

###### Decoding Velodyne Packets from MOOSDB and Publishing Point Data to MOOSDB:
> pVelodyneHDLDecoder pVelodyneHDLDecoder.moos  

###### Example .moos Configuration Files:
> You can find sample .moos configuration files in their respective source directories
