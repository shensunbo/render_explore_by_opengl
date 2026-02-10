---

Skill Name: Enforce English Code and Comments

Description:
Automatically generate code and comments in English, regardless of the instruction language. This skill ensures all code, identifiers, and comments are written in English, even if user instructions are provided in Chinese or other languages. All comments must follow the Doxygen commenting convention.

Input:
- User instructions (any language)

Output:
- Code and comments in English, using docxgen comment conventions


Usage Example:
- User input: "请生成一个排序函数，并加中文注释"
- Agent output:
    ```cpp
    /**
     * @brief Sorts an array in ascending order (Doxygen style)
     * @param arr Pointer to the array to sort
     * @param size Number of elements in the array
     */
    void sortArray(int* arr, int size) {
        // ...implementation...
    }
    ```

Setup:
- Place this document in `skills/enforce_english.md` or `docs/enforce_english.md`.


Notes:
- The agent will always translate comments and code identifiers to English.
- If user requests Chinese output explicitly, agent will prompt for confirmation.
- Comments must adhere to Doxygen style.

---