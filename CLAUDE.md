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

**Quick build (Windows):**
```bash
# Configure once
cmake -B build -DCMAKE_BUILD_TYPE=Debug

# Incremental builds
build.bat
```

**Full CMake build:**
```bash
# Configure (out-of-source build)
cmake -B build -DCMAKE_BUILD_TYPE=Release

# Build
cmake --build build --config Release

# The executable will be in build/ or build/Release/ depending on generator
```

**Platform-specific notes:**
- Windows: Uses MinGW or MSVC. Qt AxContainer is automatically linked for COM/ActiveX support. Use `build.bat` for quick incremental builds.
- Linux: Standard Qt 6 build, no special dependencies beyond Qt modules.
- Qt 5 compatibility exists but Qt 6 is preferred.

**Third-party dependencies:**
- QXlsx (bundled in `3rdparty/qxlsx/`) — Excel file I/O
- Qt modules: Core, Gui, Widgets, Concurrent, PrintSupport, Charts (optional)

## Code Architecture

### Module Organization

The codebase follows a modular architecture with PascalCase directory names for major components:

**1. Excel Parsing** ([ExcelParser.h](include/ExcelParsing/ExcelParser.h), [ExcelParser.cpp](src/ExcelParsing/ExcelParser.cpp))
   - Parses .xlsx files using QXlsx
   - Header-name-based column mapping (order-agnostic)
   - Implements carry-down rules for Supplier/Address
   - Validates and populates `PriceTag` objects

**2. Excel Generation** (modular design with specialized components)
   - [ExcelGenerator.h](include/ExcelGeneration/ExcelGenerator.h) — main orchestrator
   - [ExcelLayout.h](include/ExcelGeneration/ExcelLayout.h) — grid/geometry computations
   - [ExcelFormats.h](include/ExcelGeneration/ExcelFormats.h) — QXlsx formats/styles
   - [ExcelUtils.h](include/ExcelGeneration/ExcelUtils.h) — text helpers, padding, splitting
   - [ExcelWriters.h](include/ExcelGeneration/ExcelWriters.h) — row writers
   - [ExcelRenderer.h](include/ExcelGeneration/ExcelRenderer.h) — renderTag orchestrator
   - Exports tags to XLSX using QXlsx with exact mm-to-cell conversion
   - Sets print area, page setup, column widths/row heights in mm equivalents

**3. Word Generation** ([WordGenerator.h](include/WordGeneration/WordGenerator.h), [WordGenerator.cpp](src/WordGeneration/WordGenerator.cpp))
   - Exports tags to DOCX (Office Open XML)
   - Lays out tags on A4 pages with exact mm-to-points conversion
   - Uses current `TagTemplate` for styling

**4. UI Layer** (located in `include/UI/` and `src/UI/`)
   - [MainWindow.h](include/UI/mainwindow.h) — central UI controller, manages tabs (Main, Statistics, Template Editor), drag-and-drop, theme toggle, language switch
   - [ThemeManager.h](include/UI/thememanager.h) — singleton managing light/dark themes, applies Qt stylesheets app-wide, persists via QSettings
   - [TagTemplate.h](include/UI/tagtemplate.h) — stores tag geometry (mm), page margins, spacing, per-field text styles, JSON serialization
   - [TemplateEditorDialog.h](include/UI/templateeditordialog.h) — visual editor for tag templates
   - [TrimmedHitToolButton.h](include/UI/trimmedhittoolbutton.h) — custom button widget
   - [PixmapUtils.h](include/UI/pixmaputils.h) — image utilities

**5. Template Editor** (split into specialized modules in `src/TemplateEditor/`)
   - TemplateEditor_Facade.cpp — main interface
   - TemplateEditor_Ui.cpp — UI construction and widgets
   - TemplateEditor_Renderer.cpp — preview rendering
   - TemplateEditor_Interactions.cpp — user interactions and event handling
   - TemplateEditor_Localization.cpp — bilingual support

**6. Data Models** ([pricetag.h](include/Models/pricetag.h), [pricetag.cpp](src/Models/pricetag.cpp))
   - Data model for a single price tag
   - Fields: brand, category, gender, price, price2, material, article, supplier, address, etc.
   - Service methods: `getFormattedCategory()`, `hasDiscount()`, `getDiscountPrice()`

**7. Configuration** ([configmanager.h](include/ConfigsManager/configmanager.h), [configmanager.cpp](src/ConfigsManager/configmanager.cpp))
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

**Mandatory:** Follow `styles/DrunkDogStyle.clang-format` exactly. This is enforced by team guidelines and `.cursor/rules/styles.mdc`.

**Key style directive:** You are a 25-year C++ veteran who loves following clear style guides. Consistently apply all formatting rules before committing code.

## Development Workflow (Cursor Rules Integration)

This project uses a structured workflow documented in `.cursor/rules/workflow.mdc`:

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

**Logging and Documentation:**
- **DevelopmentLog.md format:** `[<Timestamp>] <Action> - <Result>`
- **BugLog.md format:** `[<Timestamp>] <Error description> - <Result>`
- Record ALL actions in DevelopmentLog.md to synchronize with other developers/agents
- Record ALL errors (fixed or not) in BugLog.md

**Completing Tasks:**
Mark tasks complete in `/docs/ProjectTitleByCurrentCR_Implementation.md` ONLY when:
- All functionality is implemented and tested
- Code complies with project structure and style (clang-format applied)
- UI/UX matches specifications
- No errors or warnings remain

**Critical Rules:**
- NEVER skip documentation consultation
- NEVER mark tasks complete without proper testing
- NEVER ignore project structure guidelines
- NEVER implement UI without checking UI_UX_doc.md
- ALWAYS record actions in DevelopmentLog.md
- ALWAYS check BugLog.md before fixing errors
- ALWAYS auto-create missing workflow .md files with proper format

## Git Workflow (Optional)

Git automation is documented in `.cursor/rules/git.mdc` (alwaysApply: false). Key points if enabled:

**Versioning Format:**
```
<BranchName> Version <Release>.<Stage>.<Feature>.<Patch> - <Commit Message>
```
- Release: default 0
- Stage: default 4 (confirm with developer)
- Feature: tracks completed functionality (confirm with developer)
- Patch: increments by 1 per commit

**Safety Rules:**
- NEVER touch `common/` or `proto/` subprojects (managed manually)
- NEVER auto-resolve merge conflicts (prompt developer)
- ALWAYS confirm version number and commit message before committing
- ALWAYS `git fetch` and check for remote changes before push
- Update `/docs/WorkflowLogs/GitLog.md` after successful push

## Important Documentation Files

- **[PRD.md](docs/PRD.md)** — Product Requirements Document
- **[CR.md](docs/CR.md)** — Change Requests
- **[Implementation.md](docs/Implementation.md)** — Implementation plan with task tracking
- **[project_structure.md](docs/project_structure.md)** — Folder hierarchy and module organization
- **[UI_UX_doc.md](docs/UI_UX_doc.md)** — Design system and UI specifications
- **[ExcelAPI.txt](docs/ExcelAPI.txt)** — Excel input format and parsing rules (CRITICAL for parser changes)
- **[WorkflowLogs/BugLog.md](docs/WorkflowLogs/BugLog.md)** — Known issues and fixes
- **[WorkflowLogs/DevelopmentLog.md](docs/WorkflowLogs/DevelopmentLog.md)** — Action history
- **[WorkflowLogs/UserInteractionLog.md](docs/WorkflowLogs/UserInteractionLog.md)** — User feedback

## Special Notes

**Carry-Down Logic:**
When parsing Excel, Supplier and Address values persist downward until a new non-empty value appears. This allows compact invoice formats where supplier details aren't repeated for every row.

**Category+Gender Rule:**
If the Category text is ≤ 12 characters, concatenate it with Gender in one line on the tag. If > 12 characters, omit Gender entirely. This is a business rule to prevent overcrowding small tags.

**Theme Management:**
ThemeManager is a singleton. Dark/light theme affects all windows, dialogs, and the template editor. Theme state is persisted via QSettings and restored on app launch.

**Template Persistence:**
TagTemplate objects serialize to JSON. Users can save/load custom templates. The app ships with a default template.

**Export Fidelity:**
Both DOCX and XLSX generators must match the live preview layout within ≤ 1 mm tolerance. This is a hard acceptance criterion (AC-5 in PRD).

**Qt Charts (Optional):**
If Qt Charts is available (detected by CMake), statistics tab shows pie/bar charts. If not available, statistics are text-only. The code uses `#ifdef USE_QT_CHARTS` guards.

**Bilingual UI:**
MainWindow supports English and Russian via manual dual-language strings. No Qt Linguist used. Default is English.

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
- **Don't confuse file paths:** Headers use PascalCase directories (`UI/`, `ExcelGeneration/`, `Models/`)
