#!/usr/bin/env bash

rm -f /vagrant/third-party/woody-woodpacker/third-party/libft/tools/provide-targets/provide-targets

# shield
make fclean -C /vagrant
make DEVELOPMENT=1 USE_WW=0 SYS_CC=clang-20 SYS_CXX=clang++-20 -C /vagrant copy-target PROJECT_TYPE=0 TO=/home/vagrant/

# matt
make oclean -C /vagrant
make DEVELOPMENT=0 SYS_CC=clang-20 SYS_CXX=clang++-20 -C /vagrant copy-target PROJECT_TYPE=1 TO=/home/vagrant/
