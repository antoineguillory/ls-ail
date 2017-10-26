#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>

#define ARG_FIC 1

typedef int file_descriptor_t;

bool file_exists(char * file);
char * get_permissions(mode_t mode);

int main(int argc, char* argv[]) {
    if (argc < 2) {
        perror("argc < 2\n");
        exit(EXIT_FAILURE);
    }
    if(!file_exists(argv[ARG_FIC])){
        perror("No such file or directory");
        exit(EXIT_FAILURE);
    }
    
    file_descriptor_t fd;
    ino_t             inode_fd;
    mode_t            type_fd;
    char*             permissions_fd;
    nlink_t           nb_of_links_fd;
    uid_t             uid_owner_fd;
    char*             printable_user;
    gid_t             gid_owner_fd;
    char*             printable_group;
    off_t             size_fd;
    time_t            last_modif_fd;
    char*             last_modif_printable;
    char*             name_fd = argv[ARG_FIC];

    struct stat file_stat;
    struct passwd* user;
    struct group* grp;
    
    fd = (file_descriptor_t) open(argv[ARG_FIC], O_RDONLY);
    
    if (fd<0) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    //Récupération des informations sur le fichier avec fstat
    int ret_val = fstat(fd, &file_stat);
    if (ret_val < 0) {  
        perror("fstat");
        exit(EXIT_FAILURE);
    } 
    
    inode_fd       = file_stat.st_ino;
    type_fd        = file_stat.st_mode;
    permissions_fd = get_permissions(type_fd);
    nb_of_links_fd = file_stat.st_nlink;
    uid_owner_fd   = file_stat.st_uid;
    gid_owner_fd   = file_stat.st_gid;
    size_fd        = file_stat.st_size;
    last_modif_fd  = file_stat.st_mtime;
    last_modif_printable = ctime(&last_modif_fd);
    
    /*Necessaire, ctime renvoie un carriage return dans sa chaine de retour
    Nous voulons un comportement qui se rapproche de ls, ce \n est superflu.*/
    last_modif_printable[strlen(last_modif_printable)-1] = '\0';
    
    user = getpwuid(uid_owner_fd);
    printable_user = user->pw_name;
    
    grp = getgrgid(gid_owner_fd);
    printable_group = grp->gr_name;
    
    //inode perms links owner grpowner size lastmod name (do like ls -ail [filename])
    printf("%lu %s %ld %s %s %ld %s %s \n", inode_fd, permissions_fd, 
           nb_of_links_fd, printable_user, printable_group, size_fd,
           last_modif_printable, name_fd);
    return EXIT_SUCCESS;
}


bool file_exists(char* file) {
    if (access(file, F_OK ) != -1 ) {
        return true;
    } else {
        return false;
    }
}

char * get_permissions(mode_t mode){
    char* std_len = "__________";
    char* printable_permissions = malloc(strlen(std_len)+1);
    printable_permissions[0] = (S_ISDIR(mode)    ? 'd' : '-');
    printable_permissions[1] = ((mode & S_IRUSR) ? 'r' : '-');
    printable_permissions[2] = ((mode & S_IWUSR) ? 'w' : '-');
    printable_permissions[3] = ((mode & S_IXUSR) ? 'x' : '-');
    printable_permissions[4] = ((mode & S_IRGRP) ? 'r' : '-');
    printable_permissions[5] = ((mode & S_IWGRP) ? 'w' : '-');
    printable_permissions[6] = ((mode & S_IXGRP) ? 'x' : '-');
    printable_permissions[7] = ((mode & S_IROTH) ? 'r' : '-');
    printable_permissions[8] = ((mode & S_IWOTH) ? 'w' : '-');
    printable_permissions[9] = ((mode & S_IXOTH) ? 'x' : '-');
    
    return printable_permissions;
}
