

# Raspberry Pi Server


## Objective



## Introduction


## Setup Instructions

### Install Raspberry Pi OS

Install Raspberry Pi OS using Raspberry Pi Imager: [Download](https://www.raspberrypi.com/software/)

### Raspberry Pi Server Setup

#### System Update
```bash
sudo apt-get update
sudo apt-get upgrade
sudo apt-get dist-upgrade
sudo apt-get autoremove
```

#### Install Samba
```bash
sudo apt-get install samba samba-common-bin
sudo mkdir /Raspberry
sudo chmod 777 /Raspberry
sudo nano /etc/samba/smb.conf

# Add the following lines at the end of the smb.conf file:
[Raspberry]
path = /Raspberry
writeable = yes
create mask = 0777
directory mask = 0777
public = yes
```

#### Reboot the System
```bash
sudo reboot
```

#### Install Node.js and Vue CLI
```bash
curl -fsSL https://fnm.vercel.app/install | bash
source /home/pi/.bashrc
fnm use --install-if-missing 20
node -v
npm -v

npm install -g @vue/cli
vue --version
```

#### Install FastAPI and Uvicorn
```bash
sudo apt-get install python3-pip
python3 -m venv ~/env_bioreactor
source ~/env_bioreactor/bin/activate
pip install fastapi uvicorn
nano ~/.bashrc

# Add the following line at the end of the ~/.bashrc file:
source ~/env_bioreactor/bin/activate
```

#### Create Directories for Logs and Data
```bash
mkdir -p /Raspberry/Bioreactor/logs /Raspberry/Bioreactor/ServerFastAPI
mkdir -p /Raspberry/Bioreactor/logs /Raspberry/Bioreactor/ServerFastAPI/data
```

#### Configure Uvicorn
```bash
cd /Raspberry/Bioreactor/ServerFastAPI
uvicorn backend:app --host 0.0.0.0 --port 8000 
```

#### Create Uvicorn Service
```bash
sudo nano /etc/systemd/system/fastapi.service

# Add the following lines to the uvicorn.service file:
[Unit]
Description=FastAPI server
After=network.target

[Service]
User=pi
Group=www-data
WorkingDirectory=/Raspberry/Bioreactor/ServerFastAPI
Environment="PATH=/home/pi/env_bioreactor/bin"
ExecStart=/home/pi/env_bioreactor/bin/uvicorn backend:app --host 0.0.0.0 --port 8000

[Install]
WantedBy=multi-user.target


sudo systemctl daemon-reload
sudo systemctl enable uvicorn
sudo systemctl start uvicorn
sudo systemctl status fastapi
```


#### Install Unattended Upgrades
```bash
sudo apt-get install unattended-upgrades
sudo dpkg-reconfigure --priority=low unattended-upgrades
```

#### Configurer les mises à jour automatiques
```bash
sudo nano /etc/apt/apt.conf.d/50unattended-upgrades

# Add the following lines
Unattended-Upgrade::Origins-Pattern {
    "o=Debian,a=stable";
    "o=Debian,a=stable-updates";
    "o=Debian,a=proposed-updates";
    "o=Debian,a=stable-backports";
    "o=Raspbian,a=stable";
    "o=Raspbian,a=stable-updates";
    "o=Raspbian,a=proposed-updates";
    "o=Raspbian,a=stable-backports";
};
```

```bash
sudo nano /etc/apt/apt.conf.d/20auto-upgrades

# Add the following lines
APT::Periodic::Update-Package-Lists "1";
APT::Periodic::Download-Upgradeable-Packages "1";
APT::Periodic::AutocleanInterval "7";
APT::Periodic::Unattended-Upgrade "1";
```

#### Start Uvicorn
```bash
cd /Raspberry/Bioreactor/ServerFastAPI
uvicorn backend:app --host 0.0.0.0 --port 8000 

sudo systemctl enable fastapi
sudo systemctl start fastapi
sudo reboot
sudo systemctl status fastapi
```



