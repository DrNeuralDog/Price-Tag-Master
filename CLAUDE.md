# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

PriceTagMaster is a cross-platform desktop application (C++17 + Qt 6) that generates printable price tags from Excel invoice data. It reads .xlsx files with arbitrary column ordering (header-name-based mapping), applies carry-down logic for Supplier/Address, and exports A4-sized sheets to DOCX or XLSX with millimeter precision.

**Key Features:**
- Excel import with header-based parsing (no fixed column order)
- Template-based price tag design with live preview
- A4 layout calculation (tags-per-row, rows-per-page)
- Export to Word (DOCX) and Excel (XLSX)
- Light/Dark theme support
- Bilingual UI (English/Russian)

## Build System

**Build with CMake:**
```bash
# Configure (out-of-source build)
cmake -B build -DCMAKE_BUILD_TYPE=Release

# Build
cmake --build build --config Release

# The executable will be in build/ or build/Release/ depending on generator
```

**Platform-specific notes:**
- Windows: Uses MinGW or MSVC. Qt AxContainer is automatically linked for COM/ActiveX support.
- Linux: Standard Qt 6 build, no special dependencies beyond Qt modules.
- Qt 5 compatibility exists but Qt 6 is preferred.

**Third-party dependencies:**
- QXlsx (bundled in `3rdparty/qxlsx/`) — Excel file I/O
- Qt modules: Core, Gui, Widgets, Concurrent, PrintSupport, Charts (optional)

## Code Architecture

### Module Organization

**Core Classes (include/ and src/):**

1. **MainWindow** ([mainwindow.h](include/mainwindow.h), [mainwindow.cpp](src/mainwindow.cpp))
   - Central UI controller
   - Manages tabs: Main (file input), Statistics, Template Editor
   - Handles drag-and-drop, theme toggle, language switch
   - Coordinates all service classes

2. **ExcelParser** ([excelparser.h](include/excelparser.h), [excelparser.cpp](src/excelparser.cpp))
   - Parses .xlsx files using QXlsx
   - Header-name-based column mapping (order-agnostic)
   - Implements carry-down rules for Supplier/Address
   - Validates and populates `PriceTag` objects

3. **PriceTag** ([pricetag.h](include/pricetag.h), [pricetag.cpp](src/pricetag.cpp))
   - Data model for a single price tag
   - Fields: brand, category, gender, price, price2, material, article, supplier, address, etc.
   - Service methods: `getFormattedCategory()`, `hasDiscount()`, `getDiscountPrice()`

4. **TagTemplate** ([tagtemplate.h](include/tagtemplate.h))
   - Stores tag geometry (width/height in mm), page margins, spacing
   - Per-field text styles (font, size, bold, italic, alignment)
   - JSON serialization for save/load
   - Default styles for all `TagField` enum values

5. **WordGenerator** ([wordgenerator.h](include/wordgenerator.h), [wordgenerator.cpp](src/wordgenerator.cpp))
   - Exports tags to DOCX (Office Open XML)
   - Lays out tags on A4 pages with exact mm-to-points conversion
   - Uses current `TagTemplate` for styling

6. **ExcelGenerator** ([excelgenerator.h](include/excelgenerator.h), [excelgenerator.cpp](src/excelgenerator.cpp))
   - Exports tags to XLSX using QXlsx
   - Sets print area, page setup, column widths/row heights in mm equivalents

7. **TemplateEditorDialog** ([templateeditordialog.h](include/templateeditordialog.h), [templateeditordialog.cpp](src/templateeditordialog.cpp))
   - Visual editor for tag templates
   - Property panel for fonts, sizes, margins, spacing
   - Live preview widget

8. **ThemeManager** ([thememanager.h](include/thememanager.h), [thememanager.cpp](src/thememanager.cpp))
   - Singleton managing light/dark themes
   - Applies Qt stylesheets app-wide
   - Persists theme preference via QSettings

9. **ConfigManager** ([configmanager.h](include/configmanager.h), [configmanager.cpp](src/configmanager.cpp))
   - Wraps QSettings for persistent configuration
   - Stores recent files, theme, language, last template

### Data Flow

```
Excel File → ExcelParser → List<PriceTag>
                              ↓
                        MainWindow
                              ↓
         ┌────────────────────┼────────────────────┐
         ↓                    ↓                    ↓
   Statistics View    TemplateEditor     Export (DOCX/XLSX)
                              ↓
                        TagTemplate
                              ↓
                   WordGenerator / ExcelGenerator
```

### Excel Parsing Rules (ExcelAPI.txt)

**Expected Headers (Russian, case-sensitive):**
- "Поставщик" (Supplier)
- "Адрес" (Address)
- "Фирма" (Brand)
- "Категория" (Category)
- "Пол" (Gender)
- "Страна бренда" (Brand Country)
- "Место производства" (Manufacturing Place)
- "Материал" (Material)
- "Размер" (Size)
- "Артикул" (Article/SKU)
- "Цена" (Price)
- "Цена 2" (Price 2)
- "Количество" (Quantity)
- "Прочие данные" (Other Data)

**Key Rules:**
- Column order is arbitrary; only header names matter
- Missing/renamed headers are silently ignored
- Supplier and Address carry down to subsequent rows until overridden
- If Category.length() ≤ 12, combine Category + Gender; else ignore Gender
- Empty cells are skipped (not rendered on tags)

### A4 Layout Calculation

Tags are laid out on A4 (210×297 mm) using floor-based fitting:
```
N_cols = floor((210 - marginLeft - marginRight + spacingH) / (tagWidth + spacingH))
N_rows = floor((297 - marginTop - marginBottom + spacingV) / (tagHeight + spacingV))
```
Minimum: 1 column, 1 row. If tag size + margins exceed page, user must adjust.

### Units

- **Geometry:** millimeters (mm) — tag dimensions, margins, spacing
- **Fonts:** points (pt)
- **Conversion to printer/export units** is handled internally by generators

## Code Style

**Mandatory:** Follow `styles/DrunkDogStyle.clang-format` exactly. This is enforced by team guidelines.

Key style points (from Cursor rules):
- 25-year C++ veteran style
- Consistent naming, indentation, and brace placement per .clang-format
- Always format code before committing

## Development Workflow (Cursor Rules Integration)

This project uses a structured workflow documented in `.cursor/rules/`:

**Before Starting Any Task:**
1. Consult `/docs/ProjectTitleByCurrentCR_Implementation.md` for current stage and available tasks
2. Check `/docs/WorkflowLogs/BugLog.md` for known issues
3. Read `/docs/WorkflowLogs/DevelopmentLog.md` for recent actions
4. Review `/docs/project_structure.md` for structural guidance
5. Check `/docs/UI_UX_doc.md` before implementing UI changes

**Task Execution:**
- Simple subtasks: implement directly
- Complex subtasks: create a todo list first
- Always read relevant documentation links in Implementation.md before coding
- Follow UI/UX specifications for all interface changes
- Document errors in BugLog.md with format: `[<Timestamp>] <Brief description> - <Result>`
- Log completed work in DevelopmentLog.md with format: `[<Timestamp>] <Action> - <Result>`

**Completing Tasks:**
Mark tasks complete in `/docs/ProjectTitleByCurrentCR_Implementation.md` ONLY when:
- All functionality is implemented and tested
- Code complies with project structure and style
- UI/UX matches specifications
- No errors or warnings remain

**Critical Rules (from workflow.mdc):**
- NEVER skip documentation consultation
- NEVER mark tasks complete without proper testing
- NEVER ignore project structure guidelines
- NEVER implement UI without checking UI_UX_doc.md
- ALWAYS record actions in DevelopmentLog.md
- ALWAYS check BugLog.md before fixing errors

## Git Workflow (Optional, for automated git operations)

This project has detailed git rules in `.cursor/rules/git.mdc` (alwaysApply: false). Key points if git automation is enabled:

**Versioning Format:**
```
<BranchName> Version <Release>.<Stage>.<Feature>.<Patch> - <Commit Message>
```
- Release: default 0
- Stage: default 4 (confirm with developer)
- Feature: tracks completed functionality (confirm with developer)
- Patch: increments by 1 per commit

**Subproject Handling:**
- NEVER touch `common/` or `proto/` subprojects (managed manually)
- Apply same workflow to other subprojects with changes

**Pre-Commit:**
- Always `git fetch` and check for remote changes
- Prompt developer to resolve conflicts manually (never auto-resolve)
- Confirm version number and commit message with developer before committing

## Important Documentation Files

- **[PRD.md](docs/PRD.md)** — Product Requirements Document (features, acceptance criteria)
- **[Implementation.md](docs/Implementation.md)** — Implementation plan with stages and checkboxes
- **[project_structure.md](docs/project_structure.md)** — Folder hierarchy and module organization
- **[UI_UX_doc.md](docs/UI_UX_doc.md)** — Design system and UI specifications
- **[ExcelAPI.txt](docs/ExcelAPI.txt)** — Excel input format and parsing rules (CRITICAL for parser changes)
- **[WorkflowLogs/BugLog.md](docs/WorkflowLogs/BugLog.md)** — Known issues and fixes
- **[WorkflowLogs/DevelopmentLog.md](docs/WorkflowLogs/DevelopmentLog.md)** — Action history
- **[WorkflowLogs/UserInteractionLog.md](docs/WorkflowLogs/UserInteractionLog.md)** — User feedback and requests

## Special Notes

**Carry-Down Logic:**
When parsing Excel, Supplier and Address values persist downward until a new non-empty value appears. This allows compact invoice formats where supplier details aren't repeated for every row.

**Category+Gender Rule:**
If the Category text is ≤ 12 characters, concatenate it with Gender in one line on the tag. If > 12 characters, omit Gender entirely. This is a business rule to prevent overcrowding small tags.

**Theme Management:**
ThemeManager is a singleton. Dark/light theme affects all windows, dialogs, and the template editor. Theme state is persisted via QSettings and restored on app launch.

**Template Persistence:**
TagTemplate objects serialize to JSON. Users can save/load custom templates. The app ships with a default template (values in `TagTemplate::defaultStyle()` and `TagTemplate::defaultText()`).

**Export Fidelity:**
Both DOCX and XLSX generators must match the live preview layout within ≤ 1 mm tolerance. This is a hard acceptance criterion (AC-5 in PRD).

**Qt Charts (Optional):**
If Qt Charts is available (detected by CMake), statistics tab shows pie/bar charts for brand and category distribution. If not available, statistics are text-only. The code uses `#ifdef USE_QT_CHARTS` guards.

**Bilingual UI:**
MainWindow supports English and Russian via `localized(en, ru)` helper. No Qt Linguist; manual dual-language strings. Default is English.

## Development Priorities

1. **Data Integrity:** Never break Excel parsing rules (carry-down, category+gender, header-name mapping)
2. **A4 Fidelity:** Layout calculation and export accuracy are critical
3. **Code Style:** Always follow DrunkDogStyle.clang-format
4. **Documentation:** Keep workflow logs (DevelopmentLog.md, BugLog.md) up to date
5. **Testing:** Validate against sample Excel files in `docs/OtherHelpfulFiles/`

## Common Pitfalls

- **Don't assume fixed column order in Excel:** Use header names only
- **Don't forget carry-down:** Supplier/Address logic is non-trivial
- **Don't hardcode A4 dimensions:** Use `TagTemplate` mm values and layout calculation
- **Don't skip .clang-format:** Code style is enforced
- **Don't commit without logging:** Update DevelopmentLog.md and mark tasks in Implementation.md

## Resources

- Qt 6 Documentation: https://doc.qt.io
- CMake Documentation: https://cmake.org
- QXlsx (GitHub): https://github.com/QtExcel/QXlsx
- Office Open XML Spec: https://learn.microsoft.com/office/open-xml

## Contact / Support

For bugs or feature requests, see GitHub Issues (link in README.md). Internal team: consult `/docs/WorkflowLogs/` for historical context before asking questions.
