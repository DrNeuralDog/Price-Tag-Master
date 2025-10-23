# Project Structure — PriceTagMaster

## Root
PriceTagMasterProject_ver2/
- 3rdparty/
  - qxlsx/                # Bundled QXlsx library (do not modify vendored core unless upgrading)
- include/                # Public headers for core modules
  - Common.h                   # common platform helpers
  - Constants.h                # layout/measurement helpers (inline constexpr)
  - pricetag.h                 # shim -> includes models/pricetag.h
  - models/
    - pricetag.h               # domain model header
  - excelGeneration/
    - ExcelGenerator.h         # public API for Excel export
  - excelParsing/
    - ExcelParser.h            # public API for Excel parsing
  - wordGeneration/
    - WordGenerator.h          # public API for DOCX export
  - ui/
    - mainwindow.h
    - templateeditor.h
    - templateeditordialog.h
    - thememanager.h
    - trimmedhittoolbutton.h
    - pixmaputils.h
- src/                    # Implementations
  - main.cpp
  - excelParsing/
    - ExcelParser.cpp
  - wordGeneration/
    - WordGenerator.cpp
  - models/
    - pricetag.cpp             # domain model impl
  - templateEditor/
    - TemplateEditor_Facade.cpp
    - TemplateEditor_Ui.cpp
    - TemplateEditor_Renderer.cpp
    - TemplateEditor_Interactions.cpp
    - TemplateEditor_Localization.cpp
    - TemplateEditorDialog.cpp
  - ui/
    - MainWindow.cpp
    - ThemeManager.cpp
    - TrimmedHitToolButton.cpp
    - PixmapUtils.cpp
    - tagtemplate.cpp
  - excelGeneration/
    - ExcelUtils.*            # text helpers, padding, splitting
    - ExcelLayout.*           # grid/geometry computations
    - ExcelFormats.*          # QXlsx formats/styles
    - ExcelWriters.*          # row writers
    - ExcelRenderer.*         # renderTag orchestrator
    - ExcelGenerator.*        # generateExcelDocument orchestration
- resources/              # Icons and Qt resources
  - icons/
  - resources.qrc
- styles/
  - DrunkDogStyle.clang-format
- docs/
  - PRD.md
  - CR.md
  - Implementation.md
  - project_structure.md
  - UI_UX_doc.md
  - OtherHelpfulFiles/
    - ExcelAPI.txt
    - ...
  - WorkflowLogs/
    - DevelopmentLog.md
    - UserInteractionLog.md
- CMakeLists.txt
- README.md

## Module/Component Hierarchy
- UI Layer (Qt Widgets): `MainWindow`, dialogs (future: `TemplateEditorDialog`), `PreviewWidget` (planned)
- Domain Models: `Template`, `PriceTag` (fields, styles, geometry in mm)
- Services:
  - `ExcelParser` — read input workbook by headers, apply carry-down rules
  - `LayoutEngine` (planned) — compute N_cols/N_rows, pagination using mm
  - `ExcelGenerator` — export A4-constrained XLSX mirroring preview
  - `WordGenerator` — export A4-constrained DOCX mirroring preview
  - `ThemeManager` (planned) — app-wide palette/stylesheet + persistence via QSettings

## Configuration and Build
- Build System: CMake (out-of-source). Recommended directories: `cmake-build-debug-*`, `cmake-build-release/`.
- Packaging: Use `windeployqt` on Windows. Keep third-party binaries within `3rdparty/`.

## Assets and Documentation
- Images and icons: `resources/icons/`
- Style: `styles/DrunkDogStyle.clang-format` — mandatory formatting rules
- Documentation: PRD, CR, Implementation plan, UI/UX guidelines, logs under `docs/`

## Environment-specific Notes
- Windows 10+ primary target for packaging; Linux supported by Qt/CMake
- Fonts: Prefer standard system fonts; configurable via Template settings

## Conventions
- Units: geometry in mm; font sizes in pt
- Error handling: user-facing notices; details logged (planned rotating log file)
- Determinism: layout and exports share a single source of truth (LayoutEngine)

## Future Extensions
- `tests/` for unit and golden-file tests
- `tools/` for export validators and fixture generators