# Trabalho 2

# 1. Integrantes

|Nome|Matrícula|
| -- | -------- |
|Christian Hirsch Santos | 211045113 |
|Eduardo Rodrigues de Farias| 190086521 |
|Ian Lucca Soares Mesquita| 211045140 |
|Kauã Vinícius Ponte Aguiar| 211029399|

## 2. SO Usado

O sistema operacional utilizado foi o Debian, versão 12.5.0.

## 3. Aplicações demandadas na implementação da rede LAN
Para realizar a demanda, foi necessário as seguintes ferramentas:
* Configuração de rede: 
  * `ip addr`, `nano`, `cat`
  * `/etc/network/interfaces`
  * `/etc/sysctl.conf`
* Configuração de NAT: 
  * `iptables`
* Configuração de DHCP: 
  * `/etc/default/isc-dhcp-server`
  * `/etc/dhcp/dhcpd.conf` 
* Testes de validação:
  * `ping`, `arp`, `netstat`,`tracert`, `tcpdump`

## 4. Implementação da rede LAN
A princípio, foi requisitado com que fosse implementado a seguinte rede LAN com as configurações:
- Rede de acesso: 192.168.133.0/24
- Gateway: 192.168.133.1
- Rede LAN: 10.1.0.0/16

De início, se faz necessário desativar o serviço Network Manager com:

```bash
sudo service NetworkManager stop
```

E verifica-se com:

```bash
sudo service NetworkManager status
```

A resposta desse comando deve ter uma linha indicando algo como:

```
Active: inactive (dead) since ...
```

Esta linha garante que o serviço de Network Manager está inativo. Na sequência, deve-se verificar 
quais as interfaces de redes que estão instaladas no roteador, para isso, utiliza-se o comando:

```bash
ip a
```

Com essas informações, estamos prontos para começar a de fato configurar a rede.

### 4.1. Interface WAN

Deve-se inserir no arquivo o nome e senha da rede wifi. Para isso, deve-se editar o arquivo `/etc/network/interfaces` com os seguintes valores:

![Interfaces](./img/2.jpg)

E em seguida, aplicamos as alterações deste arquivo com:

```bash
sudo ifdown wlp1s0 && sudo ifup wlp1s0
```

E assim, verifica-se o efeito do comando com: 

```bash
sudo ip addr show
```

Que deve indicar que a interface wlp1s0 está com o seu estado setado como **UP**.

### 4.2 Interface LAN

Deve-se configurar um roteador para que a interface de rede LAN assuma um IP de rede de acesso
com valor **10.1.0.1** . Para isso, deve-se editar o arquivo `/etc/network/interfaces` com os seguintes valores:

![Interfaces](./img/2.jpg)

E em seguida, aplicamos as alterações deste arquivo com:

```bash
sudo ifdown enp2s0 && sudo ifup enp2s0
```

E assim, verifica-se o efeito do comando com: 

```bash
sudo ip addr show
```

Que deve indicar que a interface enp2s0 está com o seu estado setado como **UP**. Assim, 
enp2s0 está configurado com o IP **10.1.0.1**.

### 4.3 NAT

Para criar um serviço Nat e realizar o mapeamento de IPs, foi a princípio editado o arquivo **/etc/sysctl.conf** e altera a seguinte linha, setando-a para 1:

```
net.ipv4.ip_forward=1
```

Aplica-se a mudança com o comando:

```bash
sudo sysctl -p
```

Para prosseguir, é necessário limpar todas as regras de firewall presentes no equipamento, fazemos com os comandos:

```bash
sudo iptables --flush
sudo iptables --table nat --flush
sudo iptables --delete-chain
sudo iptables --table nat --delete-chain
sudo iptables -t nat -A POSTROUTING -o wlp1s0 -j MASQUERADE
sudo iptables -A FORWARD -i enp2s0 -o wlp1s0 -j ACCEPT
sudo iptables -A FORWARD -i wlp1s0 -o enp2s0 -m state --state RELATED,ESTABLISHED -j ACCEPT
```
Com esse processo, os dispositivos presentes na LAN podem acessar a internet através da interface WAN com
a garantia de que nenhuma regra irá contra seus interesses. Para prosseguir, deve-se salvar essas 
configurações com:

```bash
sudo apt-get install iptables-persistent
sudo netfilter-persistent save
sudo netfilter-persistent reload
```

### 4.4 Serviço DHCP

Após a implementação da NAT, implementa-se o serviço DHCP para prover as configurações de redes para os clientes da LAN recém criada.
A princípio, deve-se instalar o servidor DHCP com:

```bash
sudo apt-get update
sudo apt-get install isc-dhcp-server
```

Na sequência, deve-se editar o arquivo */etc/dhcp/dhcpd.conf* adicionando as seguintes configurações:

```
option subnet-mask 255.255.0.0;
option broadcast-address 10.1.0.255;
option routers 10.1.0.1;
option domain-name-servers 192.168.133.1;

subnet 10.1.0.0 netmask 255.255.0.0 {
    range 10.1.0.10 10.1.0.100;
}
```
Para conferir essa alteração, roda-se o comando:

```bash
sudo dhcpd -t
```

Que deve executar sem erros.

#### 4.4.1. Configuração de uma lease estática

Para efeito de testes, fixamos um endereço fixo a uma máquina M que se conecta na LAN criada, para isso, editamos o arquivo */etc/dhcp/dhcpd.conf* da seguinte forma:

```
host test-machine {
   hardware ethernet XX:XX:XX:XX:XX:XX; 
   fixed-address 10.1.0.14;
}
```
#### 4.4.2. Definição da Interface para o Servidor DHCP

Edita-se o arquivo */etc/default/isc-dhcp-server*, para que a sua interface *enp2s0* sempre dispare o serviço DHCP:

```
INTERFACESv4="enp2s0"  
```
#### 4.4.3. Reinicialização do serviço DHCP

Segue com os comandos:

```bash
sudo /etc/init.d/isc-dhcp-server stop
sudo /etc/init.d/isc-dhcp-server start
```

#### 4.4.4. Conferência das leases DHCP

Deve-se visualizar se o arquivo de leases providas pelo servidor DHCP, disponível em */var/lib/dhcp/dhcpd.leases* concedeu o endereço ip 10.1.0.10 à subrede criada.

### 4.5. Serviço DNAT

#### 4.5.1. Trafego DNAT

A princípio, deve-se criar uma rota de conexão para setar o tráfego DNAT, para isso, utiliza-se:

```bash
iptables -t nat -A PREROUTING -i wlp1s0 -p tcp --dport 80 -j DNAT --to-destination 10.1.0.14:8080
sudo netfilter-persistent save
sudo netfilter-persistent reload
iptables -L -v -n -t nat
```

Com isso, obtemos como resposta as rotas criadas. Para que haja a comunicação com o DNAT, utiliza-se o comando, tratado neste documento, como receptor:

```bash
sudo nc -l -p 8080
``` 

Então, deve-se criar o emissor da mensagem utilizando:

```bash
sudo echo "teste" | nc 192.168.133.155 80
```

Assim a conexão é criada, possibilitando a troca de mensagens. O receptor receberá a mensagem "
teste" provinda do emissor.

## 5. Como validar a rede LAN
### 5.1. Validações da solução
Para isso, foi realizado uma lista de validação da solução abordando WAN, LAN, NAT e seu devido 
isolamento.
Conecta-se uma máquina de teste num switch através da sua interface Ethernet. Executa-se o comando

```bash 
ipconfig
```

Na sequência, exibe-se a tabela ARP do sistema com

```bash 
sudo arp -a
```

E salva-se as respotas.

#### 5.1.1. Validando conectividade entre LAN e WAN
Utiliza-se o comando abaixo para observar a tabela de roteamento do kernel.

``` bash 
sudo netstat -r
```

#### 5.1.2. Conectividade com o gateway
Utiliza-se o comando ICMP abaixo para verificar a conectividade com a wan enviando seus pacotes para o gateway.

``` bash
ping 192.168.133.1
```

#### 5.1.3. Validar NAT

Deve-se saber se os dispositivos da LAN conseguiram acessar a internet. Utilizou-se a ferramenta tcpdump para verificar o tráfego.
Começou-se verificando a tradução de endereços da interface : enp2s0

```bash
sudo apt-get install tcpdump
sudo tcpdump -i enp2s0
```

Deve-se observar os detalhes da mensagem de resposta, verificando os campos de endereço de origem, direção do tráfego e endereço de destino para concluir que de fato o NAT está corretamente.

#### 5.1.4. Isolamento de segmento
As máquinas LAN não devem acessar outras máquinas da WAN sem passar pelo roteador e também os equipamentos externos não devem conseguir acessar os IPs da rede privada. Inicia-se o teste com o comando:

```bash
tracert fga.unb.br
```

Para verificar se um equipamento da WAN não conseguia acessar o IP da rede privada, utilizou-se de um outro computador conectado uma rede 4g e executou-se:

```bash
ping 10.1.0.14
```

Oque deve resultar em erro.

## 6. Limitações conhecidas 

Dentre as limitações conhecidas, pode-se citar a estrutura da lds. Foi um pouco difícil começar a se situar no laboratório devido a bagunça dos computadores, mas com o tempo conseguimos nos adaptar.