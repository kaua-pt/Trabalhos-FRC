# Implementação do protocolo SNTP

## Sobre o protocolo
O protocolo SNTP é um protocolo de rede, stateless, utilizado para manter relógios dos dispositivos sincronizados, ele garante a sincronia entre os timestamps das máquinas atores no protocolo. A principal ideia dele é se utilizar de um socket UDP para realizar uma requisição para um outro servidor requisitando seu horário na porta 123 do mesmo. Na sequência, intercepta-se a resposta e realiza as operações necessárias para a sincronia dos servidores.

## Relatório
### 1 - Qual sistema operacional foi usado na construção do sistema?  </br>

Para construir o sistema, o grupo utilizou as distro linux ubuntu e debian. Um dos membros do grupo utilizou o wsl para realizar algumas das tarefas propostas. Outro membro utilizou uma VM para realizar o trabalho.

</br>

### 2 - Qual ambiente de desenvolvimento foi usado ?</br>
Para desenvolver o app, utilizou-se, como já falado anteriormente, as distro linux ubuntu e debian com apoio do wsl. De IDE foi utilizado o visual studio code.
</br>

### 3 - Como construir a aplicação ? </br>

Para construir a aplicação, basta utilizar-se do comando: </br>

```
gcc -o [Arquivo de saída] ./main.c
```
Substituindo "[Arquivo de saída]" pelo nome desejado.
Exemplo de comando:

```
gcc -o SNTP ./main.c
```

### 4 - Como executar a aplicação ? </br>

Para executar a aplicação, basta executar:

```
./[Arquivo de saída] [Url para qual seja sincronizar]
```

Substituindo o arquivo de saída indicado no item anterior e a URL para sincronizar nos seus respectivos campos. Seguem algumas sugestões para teste:

    - pool.ntp.org
    - br.pool.ntp.org
    - time.google.com

Exemplo de comando:

```
./SNTP pool.ntp.org
```

### 5 - Quais são as telas (instruções de uso) ? </br>

Para utilizar o aplicativo, basta apenas builda-lo, passo indicado no item 3 e roda-lo utilizando a linha de comando como indicado no item anterior. Na sequência, ele irá realizar suas atividades printando apenas o solicitado pelo documento emitido pelo professor.
</br>

### 6 - Quais são as limitações conhecidas ?

A princípio, essas são as limitações:</br>
- Não mapeou-se todos os possíveis erros.
