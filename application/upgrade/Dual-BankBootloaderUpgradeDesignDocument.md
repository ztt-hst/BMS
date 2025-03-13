# Dual-Bank Boot-loader Upgrade Design V1.0

## 1. Flash map

|          Flash Address           | Flash Size(KB) | Block Number | Name                                |
| :------------------------------: | :------------: | :----------: | ----------------------------------- |
| **0x08000000 ------ 0x08007FFF** |     **32**     |    **16**    | **Boot-loader**                     |
| **0x08008000 ------ 0x0803FFFF** |    **224**     |   **112**    | **Application**                     |
| **0x08040000 ------ 0x08077FFF** |    **224**     |   **112**    | **Backup Application**              |
| **0x08078000 ------ 0x0807FFFF** |     **32**     |    **16**    | **Environment Variable Parameters** |

## 2. Dual-Blank Upgrade Flowchart

```mermaid
graph TD;
    A[Bootloader Startup] --> B{Check Firmware Flag}
    B -- No --> C[Boot Current Firmware]
    B -- Yes --> D[Verify Firmware]
    
    D -- Invalid --> E[Abort Boot Old Firmware]
    D -- Valid --> F[Switch to Backup]
    
    F --> G[Backup Current Application]
    G --> H[Receive New Firmware block by block]
    H --> I[Write Firmware Flag and Restart System]
    
    I --> J[Run and Validate New Firmware]
    J -- Successful --> K[Upgrade Successful, Mark New Firmware as Primary]
    J -- Failed --> L[Rollback to Old Firmware]
    
    K --> M[System Runs Normally]
    L --> M