Vagrant.configure("2") do |config|
  config.vm.box = "ubuntu/jammy64"
  config.vm.network "public_network"

  config.vm.provider "virtualbox" do |vb|
    vb.gui = true
    vb.cpus = 2
    vb.memory = "4096"

    vb.customize ["modifyvm", :id, "--vram", "128"]
    vb.customize ["modifyvm", :id, "--clipboard-mode", "bidirectional"]
    vb.customize ["modifyvm", :id, "--draganddrop", "bidirectional"]
  end

  config.vm.provision "shell", inline: <<-SHELL
    apt-get update -y
    DEBIAN_FRONTEND=noninteractive apt-get install -y build-essential git nasm wget
    DEBIAN_FRONTEND=noninteractive apt-get install -y \
      xfce4 \
      lightdm \
      mousepad \
      thunar \
      xorg \
      libx11-dev \
      libxrandr-dev \
      libxinerama-dev \
      libxcursor-dev \
      libxi-dev \
      libxext-dev

    # systemctl enable lightdm
    systemctl set-default graphical.target

    wget https://apt.llvm.org/llvm.sh
    chmod +x llvm.sh
    ./llvm.sh 20
    rm -rf llvm.sh*

    make fclean -C /vagrant
    make DEVELOPMENT=0 CC=clang-20 CXX=clang++-20 -C /vagrant copy-target TO=/home/vagrant/
  SHELL

  config.vm.provision "shell", inline: "shutdown -r now", run: "once"
end

# vi: set ft=ruby :
