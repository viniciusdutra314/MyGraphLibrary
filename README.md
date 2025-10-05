## Estrutura do Projeto

```
.
├── CMakeLists.txt          # Arquivo de configuração do CMake (com build e debug CMAKE_BUILD_TYPES)
├── LICENSE                 
├── README.md                       
├── src/
│   ├── include/
│   │   ├── data_structures.h # Definição das estruturas de dados
│   │   └── graph_library.h   # Cabeçalhos da biblioteca do grafo
│   ├── graph_library.c       # Implementação da biblioteca do grafo
│   └── main.c                # Ponto de entrada principal da aplicação CLI
└── tests/
    ├── test_suite.py         # Suíte de testes em Python
    └── graphs_for_floyd_warshall/ # Grafos de teste
        └── ...
```