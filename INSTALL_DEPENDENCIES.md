#!/usr/bin/env bash
# Installation of dependencies

Install  software packages:

       - gcc or clang or  build-essential
       - bash-builtins
       - The respective database libraries and header files
          + libpq-dev for cg_psql (i.e. Postgresql)
          + sqlite3-dev for cg_sqlite
## Ubuntu or Debian Linux:

     apt-get install build-essential bash-builtins

     apt-get install postgresql libpq-dev
            or
     apt-get install sqlite3 sqlite3-dev




## MacOSX

    You need https://www.macports.org/

    sudo port selfupdate

    sudo port install  bash

    port  search  postgresql | grep '(databases)' # Find out package name of current postgresql
    port install  postgresql94  libpqxx           # Replace postgresql94 by current postgresql package name
          or
    port install  sqlite3


Important: The builtins will work with  the bash version installed by Mac ports  /opt/local/bin/bash, but not with  /bin/bash.
You might need to change to the newly installed Bash.
The following shebang line at the top of the script might select the proper Bash version:

     #!/usr/bin/env bash
