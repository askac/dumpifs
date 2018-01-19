# dumpifs
Dirty hack dumpifs.
Modified to be compiled on most platform without QNX SQP.

For those who are interested in hacking MIB2 firmware.
The compress tool support ucl compressed image only.
Use on your own risk.

# Basic Idea
1. Get firmware  *.ifs
2. Get an decompressed image copy by -u command
3. Modify the decompressed image using hex editor
4. Compress the modified image by uuu
5. Use dd to repack it, verify using dumpifs again (some padding bytes may need to be manually added)
6. rz (Not tested yet...)

# Reference
The original code is from
https://github.com/vocho/openqnx
