#ifndef __COMPAT_H_
#define __COMPAT_H_
    #define S_IFLNK     0xA000
    #define S_IFNAM     0x5000
    #define S_IFSOCK    0xC000

	#define S_ISUID     004000              /* set user id on execution         */
	#define S_ISGID     002000              /* set group id on execution        */
	#define S_ISVTX     001000              /* sticky bit (does nothing yet)    */
	
	/*
	 *  Group permissions
	 */
	#define S_IRWXG     000070              /*  Read, write, execute/search     */
	#define S_IRGRP     000040              /*  Read permission                 */
	#define S_IWGRP     000020              /*  Write permission                */
	#define S_IXGRP     000010              /*  Execute/search permission       */
	
	/*
	 *  Other permissions
	 */
	#define S_IRWXO     000007              /*  Read, write, execute/search     */
	#define S_IROTH     000004              /*  Read permission                 */
	#define S_IWOTH     000002              /*  Write permission                */
	#define S_IXOTH     000001              /*  Execute/search permission       */
#endif