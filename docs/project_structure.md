# Project Structure — PriceTagMaster

## Root
PriceTagMasterProject_ver2/
- 3rdparty/
  - qxlsx/                # Bundled QXlsx library (do not modify vendored core unless upgrading)
- include/                # Public headers for core modules
  - excelgenerator.h
  - excelparser.h
  - mainwindow.h
  - pricetag.h
  - wordgenerator.h
- src/                    # Implementations
  - excelgenerator.cpp
  - excelparser.cpp
  - main.cpp
  - mainwindow.cpp
  - pricetag.cpp
  - wordgenerator.cpp
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