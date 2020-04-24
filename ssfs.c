#define FUSE_USE_VERSION 28
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/xattr.h>
#include <sys/wait.h>

static const char *dirpath = "/home/opi/rahasia";
char list[100]="9(ku@AW1[Lmvgax6q`5Y2Ry?+sF!^HKQiBXCUSe&0M.b%rI'7d)o4~VfZ*{#:}ETt$3J-zpc]lnh8,GwP_ND|jO";
char encv1[] = "encv1_";


void enc(char* encrypt)
{
	if(!strcmp(encrypt,".") || !strcmp(encrypt,"..")) return;
	int enkripsi = strlen(encrypt);
	for(int i=0;i<enkripsi;i++)
	{
		for(int j=0;j<84;j++)
		{
			if(encrypt[i]==list[j])
			{
				encrypt[i] = list[(j+10)%84];
				break;
			}
		}
	}
}

void dec(char* decrypt)
{
	if(!strcmp(decrypt,".") || !strcmp(decrypt,"..")) return;
	int dekripsi = strlen(decrypt);
	for(int i=0;i<dekripsi;i++)
	{
		for(int j=0;j<84;j++){
			if(decrypt[i]==list[j]){
				decrypt[i] = list[(j+74)%84];
				break;
			}
		}
	}
}
static int xmp_getattr(const char *path, struct stat *stbuf)
{
  int res;
	char fpath[1000];
	char name[1000];
	sprintf(name,"%s",path);
	enc(name);
	sprintf(fpath, "%s%s",dirpath,name);
	printf("getattr custom %s\n", fpath);
	if (strstr(fpath, encv1)){
	char newNamePath[1000];
	int len = strlen(fpath);
	strcpy(newNamePath, fpath);
	newNamePath[len - 4] = '\0';
  	//char *ext = &fpath[len - 4];
		// if (strcmp(ext, iz1) == 0){
		// 	res = lstat(newNamePath, stbuf);
		// }else{
		// 	res = lstat(fpath, stbuf);
		// }  	
    res = lstat(fpath, stbuf);
	
	if (res != 0){
		return -ENOENT;
	}

	return 0;
}

static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
		       off_t offset, struct fuse_file_info *fi){
	int res;
    DIR *dp;
	struct dirent *de;

	(void) offset;
	(void) fi;
	char fpath[1000];
	char name[1000];
	if (strcmp(path, "/") == 0){
		sprintf(fpath, "%s", dirpath);
	}else{
		sprintf(name,"%s",path);
		enc(name);
		sprintf(fpath, "%s%s",dirpath,name);
	}
	dp = opendir(fpath);
	if (dp == NULL)
		return -errno;

	while ((de = readdir(dp)) != NULL) {
		struct stat st;
		memset(&st, 0, sizeof(st));
		st.st_ino = de->d_ino;
		st.st_mode = de->d_type << 12;
		if (filler(buf, de->d_name, &st, 0)) break;
	}
		// char fullpathname[1000];
		// sprintf(fullpathname, "%s/%s", fpath, de->d_name);
		// char temp[1000];
		// strcpy(temp,de->d_name);
		// dec(temp);
        // enc(temp);
	closedir(dp);
	return 0;
}

static int xmp_mkdir(const char *path, mode_t mode)
{
	int res;
	char fpath[1000];
	char name[1000];
	sprintf(name,"%s",path);
	enc(name);
	sprintf(fpath, "%s%s",dirpath,name);
	printf("mkdir %s\n", fpath);
	res = mkdir(fpath, mode);

	if (res == -1)
		return -errno;

	return 0;
}

static  struct fuse_operations xmp_oper = {
    .getattr = xmp_getattr,
    .readdir = xmp_readdir,
    .mkdir = xmp_mkdir,
    
};

int main(int argc, char *argv[])
{
	umask(0);
	return fuse_main(argc, argv, &xmp_oper, NULL);
}
