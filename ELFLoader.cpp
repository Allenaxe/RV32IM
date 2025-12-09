#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <cstdint>

// 如果你的系統沒有 <elf.h>，你需要手動定義這些結構
// 這裡為了完整性，列出 RV32 必要的結構定義
typedef uint32_t Elf32_Addr;
typedef uint32_t Elf32_Off;
typedef uint16_t Elf32_Half;
typedef uint32_t Elf32_Word;

#define EI_NIDENT 16
#define PT_LOAD   1
#define EM_RISCV  243

// 1. ELF 文件頭
struct Elf32_Ehdr {
    unsigned char e_ident[EI_NIDENT]; // Magic number and info
    Elf32_Half    e_type;
    Elf32_Half    e_machine;          // 應該是 243 (RISC-V)
    Elf32_Word    e_version;
    Elf32_Addr    e_entry;            // 程式入口點 (Entry Point) !! 重要
    Elf32_Off     e_phoff;            // Program Header Table 的偏移量 !! 重要
    Elf32_Off     e_shoff;
    Elf32_Word    e_flags;
    Elf32_Half    e_ehsize;
    Elf32_Half    e_phentsize;        // 每個 Program Header 的大小
    Elf32_Half    e_phnum;            // Program Header 的數量 !! 重要
    Elf32_Half    e_shentsize;
    Elf32_Half    e_shnum;
    Elf32_Half    e_shstrndx;
};

// 2. 程式頭 (描述一個 Segment)
struct Elf32_Phdr {
    Elf32_Word    p_type;             // 段類型 (只關心 PT_LOAD = 1)
    Elf32_Off     p_offset;           // 段在檔案中的偏移量
    Elf32_Addr    p_vaddr;            // 段在記憶體中的虛擬地址 (Physical addr 常用此值)
    Elf32_Addr    p_paddr;
    Elf32_Word    p_filesz;           // 段在檔案中的大小
    Elf32_Word    p_memsz;            // 段在記憶體中的大小 (可能大於 filesz)
    Elf32_Word    p_flags;            // 權限 (R/W/X)
    Elf32_Word    p_align;
};

// 模擬的記憶體介面 (假設)
class SimMemory {
public:
    void write_byte(uint32_t addr, uint8_t val) {
        // 在這裡實作寫入你的模擬器記憶體陣列
        // 例如: ram[addr] = val;
        printf("Mem[%08x] = %02x\n", addr, val); 
    }
};

// ==========================================
// ELF 載入器函數
// ==========================================
bool load_elf(const char* filename, SimMemory* mem, uint32_t* entry_point) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Error: Cannot open file " << filename << std::endl;
        return false;
    }

    // 1. 讀取 ELF Header
    Elf32_Ehdr ehdr;
    file.read(reinterpret_cast<char*>(&ehdr), sizeof(ehdr));

    // 2. 驗證 Magic Number (0x7F 'E' 'L' 'F')
    if (ehdr.e_ident[0] != 0x7F || ehdr.e_ident[1] != 'E' || 
        ehdr.e_ident[2] != 'L' || ehdr.e_ident[3] != 'F') {
        std::cerr << "Error: Not a valid ELF file." << std::endl;
        return false;
    }

    // 3. 驗證是否為 RISC-V 架構 (EM_RISCV = 243)
    // 注意：某些舊工具鏈可能使用不同的 ID，但現代工具鏈皆為 243
    if (ehdr.e_machine != EM_RISCV) {
        std::cerr << "Error: Not a RISC-V executable." << std::endl;
        return false;
    }

    // 4. 定位到 Program Header Table
    file.seekg(ehdr.e_phoff, std::ios::beg);

    // 5. 遍歷每一個 Program Header
    for (int i = 0; i < ehdr.e_phnum; ++i) {
        Elf32_Phdr phdr;
        file.read(reinterpret_cast<char*>(&phdr), sizeof(phdr));

        // 我們只關心 LOAD 類型的段 (PT_LOAD = 1)
        if (phdr.p_type == PT_LOAD) {
            
            // 保存當前文件位置，因為我們要跳去讀數據
            std::streampos current_pos = file.tellg();

            // A. 將檔案中的內容載入記憶體 (程式碼 + 初始化數據)
            file.seekg(phdr.p_offset, std::ios::beg);
            
            std::vector<uint8_t> buffer(phdr.p_filesz);
            if (phdr.p_filesz > 0) {
                file.read(reinterpret_cast<char*>(buffer.data()), phdr.p_filesz);
                
                for (size_t j = 0; j < phdr.p_filesz; ++j) {
                    mem->write_byte(phdr.p_vaddr + j, buffer[j]);
                }
            }

            // B. 處理 BSS (未初始化數據區)
            // 如果 memsz > filesz，剩下的空間需要填 0
            if (phdr.p_memsz > phdr.p_filesz) {
                uint32_t bss_size = phdr.p_memsz - phdr.p_filesz;
                uint32_t bss_start = phdr.p_vaddr + phdr.p_filesz;
                
                for (size_t j = 0; j < bss_size; ++j) {
                    mem->write_byte(bss_start + j, 0);
                }
            }

            // 恢復文件位置以讀取下一個 Header
            file.seekg(current_pos);
        }
    }

    // 6. 設定 Entry Point
    *entry_point = ehdr.e_entry;
    std::cout << "ELF loaded. Entry point: 0x" << std::hex << *entry_point << std::endl;

    return true;
}

int main() {
    SimMemory ram;
    uint32_t pc;
    
    // 使用範例
    if (load_elf("hello", &ram, &pc)) {
        // 開始模擬...
        // cpu.pc = pc;
        std::cout << pc << '\n';
    }
    return 0;
}