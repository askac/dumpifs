# dumpifs
Dirty hack dumpifs.
Modified to be compiled on most platform without QNX SQP.

For those who are interested in hacking MIB2 firmware.
The compress tool support ucl and lzo compressed images only.
Use on your own risk.

# Basic Idea
1. Get firmware  *.ifs.
2. Get an decompressed image copy by -u command
3. Modify the decompressed image using hex editor to bypass login (for example, the login script)
4. Compress the modified image by uuu or zzz (depends on original modified type)
5. Use dd to repack it, verify using dumpifs again (some padding bytes may need to be manually added). You can also use the helper script to do that
6. Get into IPL, rz to retrive the modified ifs and get into the emergency shell. You might want to kill MIBEmergency (directly modify the binary by hex editor or slay command after boot) to stop the emergency boot after seconds which will lock you out again. or you can replace the original shadow file in system with yours.

# Make

```bash
git clone https://github.com/askac/dumpifs.git
chmod +x dumpifs
./dumpifs
```
Available on the AUR: [https://aur.archlinux.org/packages/dumpifs-git/](https://aur.archlinux.org/packages/dumpifs-git/)

# Options

```
%C - dump an image file system

%C	[-mvxbz -u file] [-f file] image_file_system_file [files]
 -b       Extract to basenames of files
 -u file  Put a copy of the uncompressed image file here
 -v       Verbose
 -x       Extract files
 -m       Display MD5 Checksum
 -f file  Extract named file
 -z       Disable the zero check while searching for the startup header.
          This option should be avoided as it makes the search for the
          startup header less reliable.
          Note: this may not be supported in the future.
```

# Reference
The original code is from
https://github.com/vocho/openqnx
