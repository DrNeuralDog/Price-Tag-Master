# Implementation Plan for PriceTagMaster

## Feature Analysis
### Identified Features
- Excel input parsing by header names with carry-down rules and optional fields (FR-1…FR-6)
- Replication of tags per row (FR-7)
- Tag Template Editor: editable fields, fonts, sizes, alignment, borders, geometry (FR-8)
- Live Preview: paginated A4 sheet, zoom, navigation (FR-9)
- A4-constrained export to DOCX and XLSX with layout fidelity (FR-10, FR-12)
- Layout calculation for tags-per-row and rows-per-page considering margins/spacing (FR-11)
- Light/Dark theme toggle applied app-wide (FR-13)
- UX: file chooser, drag-and-drop plus button, non-blocking parsing with notices (FR-14, FR-16)

### Feature Categorization
- Must-Have Features:
  - Excel import and parsing with carry-down and header-based mapping (FR-1…FR-6)
  - Layout engine in mm, A4 constraint, pagination (FR-9…FR-11)
  - Export to XLSX and DOCX with ≤ 1 mm fidelity (FR-10, FR-12)
  - Template Editor and Live Preview (FR-8, FR-9)
  - Theme toggle (FR-13)
- Should-Have Features:
  - UX notices for missing headers and validation (FR-16)
  - Drag-and-drop and plus button flow (FR-14)
- Nice-to-Have Features:
  - Preset templates library, snap-to-grid, optional images in templates (future)
  - Localization beyond English (FR-15 – future)

## Recommended Tech Stack
### Frontend (Desktop UI)
- Framework: Qt 6 Widgets (C++17) — lightweight native desktop UI, excellent graphics (QGraphicsView), cross-platform.
- Documentation: [Qt 6 Documentation](https://doc.qt.io)

### Core Logic
- Language: C++17 — performance and determinism; clear ownership; cross-platform.
- Build: CMake (out-of-source) — standard, IDE-friendly.
- Documentation: [CMake Documentation](https://cmake.org)

### Data and Persistence
- Input: XLSX via bundled QXlsx.
- Templates: JSON via Qt JSON (QJsonDocument/QJsonObject).
- Settings: QSettings for theme and recent files.
- Documentation: [QSettings](https://doc.qt.io/qt-6/qsettings.html), [Qt JSON](https://doc.qt.io/qt-6/qjsondocument.html)

### XLSX Export
- Library: QXlsx (bundled under `3rdparty/qxlsx`) for column widths, row heights, print area, page setup.
- Documentation: [QXlsx (GitHub)](https://github.com/QtExcel/QXlsx)

### DOCX Export
- Approach: Minimal Open XML writer (ZIP + XML parts) or small third-party library hidden behind `WordGenerator` abstraction.
- Documentation: [Office Open XML | Microsoft Learn](https://learn.microsoft.com/office/open-xml)

### Additional Tools
- Testing: Qt Test for unit tests of parsing/layout.
- Packaging: Qt deployment tools (windeployqt on Windows).
- Versioning: Git (branch-per-stage recommended).

## Implementation Stages

### Stage 1: Foundation & Setup
Duration: 2–3 days (with 4× buffer in planning)
Dependencies: None

Sub-steps:
- [ ] Configure CMake targets, out-of-source builds, and `3rdparty/qxlsx` integration
- [ ] Establish logging and error reporting approach
- [ ] Implement `ThemeManager` and UI toggle wiring placeholder
- [ ] Implement input file selection, drag-and-drop stub, and header validation notice flow
- [ ] Define `Template` data model (geometry in mm, fields, styles) and JSON schema

### Stage 2: Core Features (MVP)
Duration: 5–8 days (with 4× buffer)
Dependencies: Stage 1

Sub-steps:
- [ ] Implement layout engine in mm (A4, margins, spacing, N_cols/N_rows, pagination)
- [ ] Implement `PreviewWidget` using QGraphicsView with zoom and page navigation
- [ ] Implement XLSX export: mm→points→Excel units; set print area, margins; disable scaling
- [ ] Implement parsing per Excel API: header mapping, carry-down, optional fields, Category+Gender rule

### Stage 3: Advanced Features
Duration: 5–8 days (with 4× buffer)
Dependencies: Stage 2

Sub-steps:
- [ ] Implement `TemplateEditorDialog` with property panel and live preview binding
- [ ] Implement DOCX export with fixed A4 page composition mirroring preview
- [ ] JSON save/load of templates; starter presets library
- [ ] Persist UI theme and recent files via QSettings

### Stage 4: Polish & Optimization
Duration: 4–6 days (with 4× buffer)
Dependencies: Stage 3

Sub-steps:
- [ ] Comprehensive tests: parsing, layout edges, golden files for XLSX/DOCX
- [ ] UX: notices for missing headers, non-blocking parsing, clear error banners
- [ ] Performance: parse 5k×20 < 3s; preview memory < 300 MB
- [ ] Packaging for Windows (windeployqt) and Linux (as applicable)

## Dependencies and Risks
- DOCX fidelity and library selection — mitigate by abstracting `WordGenerator` and prototyping early.
- Cross-DPI rendering consistency — use mm as source of truth; shared layout between preview and exports.
- Font availability — prefer standard fonts; allow user-configurable font in template.

## Acceptance Criteria (condensed)
- Theme applies app-wide without glitches (AC-6)
- Changing tag size updates grid; exports match preview within ≤ 1 mm (AC-2, AC-5)
- Valid workbook renders correct tags honoring data rules (AC-1…AC-4)

## Resource Links
- [Qt 6 Documentation](https://doc.qt.io)
- [CMake Documentation](https://cmake.org)
- [QXlsx (GitHub)](https://github.com/QtExcel/QXlsx)
- [Office Open XML | Microsoft Learn](https://learn.microsoft.com/office/open-xml)
