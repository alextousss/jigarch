# jigarch
Manage your files in basicly encrypted archives


To add a file/folder to an archive           : ' jigarch -crypt <path to archive> <path to file/folder> '

To decrypt a file/folder from an archive     : ' jigarch -decrypt <path to archive> <path to destination folder> <file/folder to decrypt> '

jigarch is still currently in developpement, use it at your own risks !


Archives are files with the content of multiple files, with a directory system and a very basic encryption(1) system.
It's an easy way to make sure that *non-sensitive* data on a USB key will be *harder* to read if lost.






(1) *It will surely block average people, but anyone who can write some code to bruteforce the encryption will get the passphrase quickly, just look at the encryption algorithm and you will quickly get why. It's made to be simple, not secure.*
