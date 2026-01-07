Vagrant.configure("2") do |config|
  config.vm.box = "ubuntu/jammy64"
  config.vm.network "public_network"
  config.vm.network "forwarded_port", guest: 4242, host: 4242, auto_correct: true, protocol: "tcp"

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
    DEBIAN_FRONTEND=noninteractive apt-get install -y build-essential git nasm wget jq ffmpeg btop
    DEBIAN_FRONTEND=noninteractive apt-get install -y \
      xfce4 \
      lightdm \
      lightdm-gtk-greeter \
      mousepad \
      thunar \
      xorg \
      libx11-dev \
      libxrandr-dev \
      libxinerama-dev \
      libxcursor-dev \
      libxi-dev \
      libxext-dev

    rm -f /bin/sh
    ln -s /bin/bash /bin/sh

    systemctl enable lightdm
    systemctl set-default graphical.target

    wget https://apt.llvm.org/llvm.sh
    chmod +x llvm.sh
    ./llvm.sh 20
    rm -rf llvm.sh*

    bash /vagrant/make/vagrant-build.sh
  SHELL

  config.vm.provision "shell", inline: "shutdown -r now", run: "once"
end

# vi: set ft=ruby :
