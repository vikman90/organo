from sys import argv
from spwd import getspnam
from crypt import crypt

try:
    pwd = getspnam(argv[1]).sp_pwd
except KeyError:
    exit(1)

exit(0 if crypt(argv[2], pwd[:11]) == pwd else 1)
