# Servidor de Log UDP

Este projeto implementa um servidor de log UDP simples que escuta em uma porta especificada e salva os logs recebidos em um arquivo. O servidor pode ser gerenciado usando comandos simples, como iniciar, parar e verificar o status do serviço.

## Funcionalidades

- **Escuta de logs UDP**: O servidor escuta pacotes UDP em uma porta específica e grava os dados recebidos em um arquivo de log.
- **Comando de controle**: O servidor pode ser controlado via comandos:
  - `start` para iniciar o servidor.
  - `stop` para parar o servidor.
  - `status` para verificar o status do servidor.

## Pré-requisitos

- Linux com suporte a **systemd**.
- Permissões adequadas para adicionar um serviço **systemd**.
- Um ambiente de rede configurado para enviar logs via UDP para o servidor.

## Instalação

1. **Clone o repositório** para sua máquina local:

    ```bash
    git clone https://github.com/SeuUsuario/logserver.git
    cd logserver
    ```

2. **Compilação do código**:

    Se o código estiver em C, compile o servidor de logs:

    ```bash
    gcc -o logserver logserver.c
    ```

    Certifique-se de que o código esteja no arquivo `logserver.c` (ou ajuste conforme necessário).

3. **Configuração do serviço**:

    Para configurar o serviço **systemd**, mova o arquivo de serviço `logserver.service` para `/etc/systemd/system/`
   

5. **Habilitar e iniciar o serviço**:

    Após salvar o arquivo de serviço, recarregue o **systemd** e habilite o serviço:

    ```bash
    sudo systemctl daemon-reload
    sudo systemctl enable logserver.service
    sudo systemctl start logserver.service
    ```

6. **Verificar o status**:

    Verifique se o serviço está em execução:

    ```bash
    sudo systemctl status logserver.service
    ```

    Se tudo estiver funcionando corretamente, o servidor de logs UDP estará pronto para receber dados.

## Configuração do Roteador

Para que o seu roteador envie logs para o servidor, configure o roteador para enviar os logs via UDP para a porta que você configurou no servidor (por padrão, porta 514). Certifique-se de que o firewall do seu servidor permita conexões UDP na porta configurada.

## Comandos de Controle

- **Iniciar o servidor**:

    ```bash
    sudo systemctl start logserver.service
    ```

- **Parar o servidor**:

    ```bash
    sudo systemctl stop logserver.service
    ```

- **Verificar o status do servidor**:

    ```bash
    sudo systemctl status logserver.service
    ```

- **Habilitar o servidor para iniciar automaticamente com o sistema**:

    ```bash
    sudo systemctl enable logserver.service
    ```

## Logs

Os logs recebidos do servidor são gravados no diretório configurado. O arquivo de log pode ser encontrado em `/var/log/logserver.log` ou no local especificado no código.

## Contribuição

Se você deseja contribuir para este projeto, fique à vontade para fazer um **fork** do repositório e enviar **pull requests**. Certifique-se de seguir as melhores práticas de codificação e incluir testes quando possível.

## Licença

Este projeto está licenciado sob a Licença MIT - veja o arquivo [LICENSE](LICENSE) para mais detalhes.
