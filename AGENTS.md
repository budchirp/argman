# Code Standards

## Naming Conventions

- **Classes/Enums**: `PascalCase` (e.g., `TokenType`, `Lexer`)
- **Functions/Methods**: `snake_case` (e.g., `next_token`, `read_char`)
- **Variables/Parameters**: `snake_case` (e.g., `byte_position`, `token_type`)
- **Private Members**: `snake_case` (same as variables, no suffix)
- **Files**: `snake_case` (e.g., `token_type.cppm`, `lexer.cppm`)
- **Enum Values**: `PascalCase` (e.g., `TokenType::Identifier`)

## Code Style

- Do not add comments to code
- Always write explicit `private:` and `public:` sections in classes, with `private:` first
- Use `auto` when the type can be deduced
- Be explicit: do not use shortcut names (e.g., `ty` for type, `ctx` for context)
- Always use classes (never structs); use explicit constructors to ensure all members are initialized
- Use C++ standard library functions over custom helpers when available (e.g., `std::isdigit`, `std::isalpha`)
- ALWAYS ASK QUESTIONS. Don't change code because there's other issues with it. Just ask questions to me and I'll guide you with the correct path.

## Project Structure

- C++26 with modules (`.cppm` extension)
- Module names follow `argman.<component>.<submodule>` pattern
