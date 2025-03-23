# **Servidor de Log UDP**  

Este projeto implementa um **servidor de log UDP** que escuta em uma porta especificada e salva os logs recebidos em um arquivo. O servidor pode ser gerenciado usando comandos simples, como **iniciar**, **parar** e **verificar o status**.  

## **Funcionalidades**  

- **Recebimento de logs via UDP**: O servidor escuta pacotes UDP em uma porta específica e grava os dados recebidos em um arquivo de log.  
- **Gerenciamento via systemd**: O servidor pode ser controlado com os seguintes comandos:  
  - `start` → Inicia o servidor.  
  - `stop` → Para o servidor.  
  - `status` → Exibe o status do servidor.  
- **Instalação simplificada via APT**  

---

## **Pré-requisitos**  

- Linux com suporte a **systemd**.  
- Permissões adequadas para adicionar um serviço **systemd**.  
- Ambiente de rede configurado para enviar logs via **UDP** para o servidor.  

---

## **Instalação**  

### **Instalação via APT**  

1. **Adicione o repositório ao APT:**  

    ```bash
    echo "deb [trusted=yes] http://francosinformatica.com/repo stable main" | sudo tee /etc/apt/sources.list.d/franco-repo.list
    ```

2. **Atualize o cache do APT:**  

    ```bash
    sudo apt update
    ```

3. **Instale o logserver:**  

    ```bash
    sudo apt install logserver
    ```

---

### **Instalação Manual (via Git)**  

1. **Clone o repositório:**  

    ```bash
    git clone https://github.com/SeuUsuario/logserver.git
    cd logserver
    ```

2. **Compilação do código:**  

    Se o código estiver em **C**, compile o servidor de logs:  

    ```bash
    gcc -o logserver logserver.c
    ```

3. **Configuração do serviço systemd:**  

    Copie o arquivo de serviço para o diretório correto:  

    ```bash
    sudo cp logserver.service /etc/systemd/system/
    ```

4. **Habilitar e iniciar o serviço:**  

    ```bash
    sudo systemctl daemon-reload
    sudo systemctl enable logserver.service
    sudo systemctl start logserver.service
    ```

5. **Verificar se o serviço está em execução:**  

    ```bash
    sudo systemctl status logserver.service
    ```

---

## **Configuração do Roteador**  

Para que o roteador envie logs para o servidor, configure-o para enviar logs via **UDP** para o IP do servidor na porta configurada (padrão: **514**).  

Caso use **iptables**, libere a porta UDP:  

```bash
sudo iptables -A INPUT -p udp --dport 514 -j ACCEPT
```

Para tornar essa configuração permanente, salve as regras do **iptables**.  

---

## **Gerenciamento do Serviço**  

- **Iniciar o servidor:**  

    ```bash
    sudo systemctl start logserver.service
    ```

- **Parar o servidor:**  

    ```bash
    sudo systemctl stop logserver.service
    ```

- **Verificar o status do servidor:**  

    ```bash
    sudo systemctl status logserver.service
    ```

- **Habilitar o serviço para iniciar automaticamente:**  

    ```bash
    sudo systemctl enable logserver.service
    ```

---

## **Logs**  

Os logs recebidos são gravados no arquivo:  

```bash
/var/log/logserver.log
```

Caso o arquivo não esteja sendo atualizado, verifique as permissões:  

```bash
sudo chmod 644 /var/log/logserver.log
```

---

## **Contribuição**  

Se deseja contribuir para o projeto, faça um **fork** do repositório e envie **pull requests**. Certifique-se de seguir boas práticas de codificação.  

---

## **Licença**  

Este projeto está licenciado sob a **Licença MIT**.  

---

