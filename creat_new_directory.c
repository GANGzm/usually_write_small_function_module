// 主要功能：获得一个指定路径的空目录
// 主要流程：需要创建的文件夹是否存在，存在时内部是否有数据，
// 			 删除数据，创建文件夹
// 模块接口：输入为路径，如“/usr/local/gzm”
//           输出为 空目录，成功返会1，失败返回负数

#include <dirent.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define DIRECTORY_PATH_LEN_MAX 256

static int customize_vsnprintf(char *str, size_t size, const char *format, va_list ap)
{
	int ret;

	if(size == 0)
		return -1;

	ret = vsnprintf(str, size, format, ap);
	str[size-1] = '\0';

	if(ret < 0) 
		return -3;

	return ret;
}

int customize_snprintf(char *str, size_t size, const char *format, ...)
{
	int ret;
	va_list ap;

	va_start(ap, format);
	ret = customize_vsnprintf(str, size, format, ap);
	va_end(ap);

	return ret;
}

static int remove_directory(const char *pathname)
{
	DIR *dir;
	struct dirent *dp;
	char all_path_buf[DIRECTORY_PATH_LEN_MAX];

	dir = opendir(pathname);
	if(dir == NULL) {
		return -1;
	}

	while((dp =readdir(dir)) != NULL) {
		if((strcmp(dp->d_name, ".") == 0) ||
				(strcmp(dp->d_name, "..") == 0)) {
			continue;
		}

		customize_snprintf(all_path_buf, DIRECTORY_PATH_LEN_MAX, "%s/%s", pathname, dp->d_name);
		
		if(dp->d_type == DT_DIR) {
			remove_directory(all_path_buf);
		} else {
			unlink(all_path_buf);
		}
	}
	
	closedir(dir);
	
	rmdir(pathname);

	return 0;
}

static inline int creat_new_directory(const char *pathname)
{
	int ret;
	
	if(access(pathname, F_OK) == 0) {
		ret = remove_directory(pathname);
		if(ret != 0) {
			return -1;
		}
	}

	ret = mkdir(pathname, 0775);
	if(ret != 0) {
		return -2;
	}
	
	return 0;
}

int main(int argc, const char *argv[])
{
	int ret;

	if(argc < 2) {
		printf("error! \n");
		return -1;
	}

	ret = creat_new_directory(argv[1]);
	if(ret != 0) {
		printf("creat new directory fail. ret=%d \n", ret);
		return -2;
	}

	return 0;
}




