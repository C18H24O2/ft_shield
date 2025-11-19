Vagrant.configure("2") do |config|
  config.vm.box = "ubuntu/focal64"
  config.vm.network "public_network"

  config.vm.provider "virtualbox" do |vb|
    vb.gui = false
    vb.cpus = 2
    vb.memory = "1024"
  end

  config.vm.provision "shell", inline: <<-SHELL
    apt-get update -y
    apt-get install -y build-essential git nasm wget clang

    wget https://apt.llvm.org/llvm.sh
    chmod +x llvm.sh
    ./llvm.sh 20
    rm -rf llvm.sh*

    make fclean -C /vagrant
    make DEVELOPMENT=0 CC=clang-20 CXX=clang++-20 -C /vagrant copy-target TO=/home/vagrant/
  SHELL
end

# vi: set ft=ruby :
