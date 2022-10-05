
@ Função para fazer o mapeamento dos pinos na memória.
@ R8 - Retorno: Endereço de memória base da GPIO. 

.equ pagelen, 4096
.equ PROT_READ, 1
.equ PROT_WRITE, 2
.equ MAP_SHARED, 1
.equ sys_mmap2, 192

.global mapMem

mapMem: 
    push{r4, r5, r7} @ Salva os registradores que usamos.
    ldr r0, =devmem
    mov r1, #0x1b0
    orr r1, #0x006
    mov r2, r1
    @ Permissão de Leitura/Escrita
    mov r7, #5
    svc 0
    movs r4, r0         @ fd for memmap
    ldr r5, =gpioaddr   @ Endereço pretendido.
    ldr r5, [r5]        @ Carrega o endereço.
    mov r1, #pagelen    @ Tamanho da memória que queremos.
    @ opções de prroteção de memória
    mov r2, #(PROT_READ + PROT_WRITE) 
    mov r3, #MAP_SHARED @ opções de compartilhamento
    mov r0, #0          @ Permite o linux escolher o endereço virtual de memória.
    mov r7, #sys_mmap2  @ Serviço mmap2
    svc 0 @ chama o serviço
    movs r8, r0 @ keep the returned virt addr
    pop {r4, r5, r7} @ Restora o valor dos registradores.
    bx lr @ Retorno da função

.data

@ Local do arquivo para puxar os endereços de memória.
devmem: .asciz "/dev/mem"
@ Endereço de memória para fazer o mapeamento dos pinos.
gpioaddr: .word 0x20200