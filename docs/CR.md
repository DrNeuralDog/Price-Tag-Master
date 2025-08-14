
## Change Request (CR): PriceTagMaster — Theme, Export, and Preview Implementation

Version: 1.0  
Date: 2025-08-08  
Owner: Project Maintainer  
Related docs: [PRD](./PRD.md), [Excel API](./OtherHelpfulFiles/ExcelAPI.txt), [Dev Log](./WorkflowLogs/DevelopmentLog.md), [User Interaction Log](./WorkflowLogs/UserInteractionLog.md)

### 1) Summary
This CR defines the scope, stages, deliverables, and acceptance criteria to complete PriceTagMaster per the PRD. It focuses on: (1) Dark/Light theme toggle, (2) Correct XLSX export of price tags, (3) Correct DOCX export of price tags, and (4) In-app preview and template editor for tag layout. The document is optimized for both human readers and AI agents executing the plan.

### 2) Background and Current State
- Baseline: Qt 6 C++ desktop app with a basic UI. Drag-and-drop of an input `.xlsx` is implemented with header validation per the Excel API. A rudimentary XLSX export exists but is not production-ready.
- Drivers: PRD v0.1 requires A4-constrained output, accurate grid layout, theming, preview, and exports to DOCX/XLSX with high fidelity.

### 3) Objectives and Success Criteria
- Provide a user-facing theme toggle (Light/Dark) applied consistently across the UI.  
  Success: Meets PRD FR-13 and AC-6.
- Implement robust XLSX export matching the previewed grid, A4 page setup, correct cell sizing, and print area; no clipping.  
  Success: Meets PRD FR-10, FR-11, FR-12 and AC-2, AC-5.
- Implement DOCX export with fixed A4 pages and an accurate grid mirroring the preview; fonts/spacing consistent.  
  Success: Meets PRD FR-10, FR-12 and AC-5.
- Deliver a template editor with live preview of paginated A4 sheets; allow editing of visible fields, fonts, sizes, alignment, tag width/height, margins, and spacing.  
  Success: Meets PRD FR-8, FR-9, FR-11 and AC-1, AC-2.

### 4) In Scope
- UI theming and persistent theme toggle.
- Layout engine producing an A4-fitted grid using mm units per PRD §6.
- XLSX and DOCX export pipelines that reproduce preview layout within ≤ 1 mm tolerance.
- Template editor + live preview (editable geometry, font, alignment, borders, visible fields).
- JSON serialization for templates (save/load).
- Non-blocking parsing and clear error/notice reporting consistent with PRD.

### 5) Out of Scope
- Printing pipeline and printer-specific dialogs (beyond producing A4-ready files).
- Non-Microsoft input/output formats.
- Advanced features like barcodes/QR (PRD §16: out of scope for v1).

### 6) Requirements Traceability (to PRD)
- Input/Parsing: FR-1, FR-2, FR-3, FR-4, FR-5, FR-6, FR-7 (baseline; reused by preview/export)
- Theming: FR-13, AC-6
- Preview/Editor: FR-8, FR-9, FR-11, AC-1, AC-2
- Export: FR-10, FR-11, FR-12, AC-2, AC-5
- UX: FR-14, FR-16; Error handling per §12

### 7) Staged Delivery Plan
Stage 1 — Dark Mode & Toggle (Priority: High)
- Implement application-wide Light/Dark themes with a visible toggle in the main UI.
- Persist choice via `QSettings`.
- Apply to all widgets/dialogs; verify contrast and disabled-state colors.
Deliverables: Theme manager, toggle control, persisted setting.  
Acceptance: AC-6.

Stage 2 — Correct XLSX Export (Priority: Critical)
- Implement A4-aware grid: compute `N_cols` and `N_rows` per PRD §6 with margins and spacing.
- Map mm to Excel column widths/row heights; set print area, page margins, and disable print scaling.
- Render tags according to the active template; skip empty fields; apply Category+Gender concatenation rule.
- Replicate tags per row count as configured.
Deliverables: Deterministic XLSX writer that mirrors preview.  
Acceptance: AC-2, AC-5; Fidelity ≤ 1 mm.

Stage 3 — Correct DOCX Export (Priority: High)
- Implement A4 fixed-page composition mirroring the same grid geometry as preview.
- Choose a lightweight DOCX approach: a minimal OpenXML writer or a vetted 3rd-party DOCX lib (allowed by PRD §10). Ensure deterministic layout and font handling.
- Render all visible fields with correct alignment and spacing; honor concatenation and carry-down rules.
Deliverables: Deterministic DOCX writer that mirrors preview.  
Acceptance: AC-5; Fidelity ≤ 1 mm.

Stage 4 — Template Editor & Live Preview (Priority: High)
- Provide a `TemplateEditorDialog` with a `QGraphicsView` canvas in mm units.
- Editable: visible fields, static text, borders, font family/size/weight, alignment, padding; tag width/height; margins; inter-tag spacing.
- JSON save/load; presets for starter templates.
- Live preview: paginated A4 sheet; zoom; next/prev page navigation.
Deliverables: Editor UI, JSON persistence, preview renderer.  
Acceptance: AC-1, AC-2.

### 8) Design Overview (for Engineers and AI Agents)
- Theming
  - Centralize theme with `ThemeManager` (palette + custom stylesheets if needed). Expose `setTheme(Theme)` and `currentTheme()`; wire a toggle in `MainWindow`.
  - Persist via `QSettings` on exit; read at startup.
- Layout Engine
  - Input: tagW/mm, tagH/mm, margins/mm, spacing/mm; A4 portrait 210×297 mm.
  - Compute: `N_cols = floor((210 - left - right + hSpacing) / (tagW + hSpacing))`, `N_rows = floor((297 - top - bottom + vSpacing) / (tagH + vSpacing))`; enforce ≥ 1.
  - Pagination: left-to-right, top-to-bottom; replicate per row config.
- XLSX Export
  - Use bundled QXlsx (see `3rdparty/qxlsx`). Map mm→points→Excel units; set column widths/row heights; define print area and margins; disable scaling.
  - Fill cells per template; keep text wrapping/alignment; avoid clipping.
- DOCX Export
  - Prefer a minimal OpenXML writer (zip + XML parts) or a small 3rd-party lib. Abstract behind `WordGenerator` to keep the app decoupled.
  - Place absolutely positioned frames or a table grid sized to mm geometry; ensure consistent fonts.
- Template Editor & Preview
  - `Template` model (JSON): geometry, spacing, margins, fields, text styles.  
  - `TemplateEditorDialog` (QWidgets) with property panel and canvas; `PreviewWidget` renders pages from the same layout engine to guarantee WYSIWYG.
- Data Rules
  - Parsing per Excel API: carry-down for Supplier/Address, optional fields ignored, Category+Gender concatenation when `len(Category) ≤ 12`.

### 9) Deliverables
- Stage 1: Theme manager + toggle + persisted setting.
- Stage 2: XLSX export module with A4 page setup and geometry mapping; unit tests.
- Stage 3: DOCX export module with A4 pages; basic font embedding/selection; sanity tests.
- Stage 4: Template editor, preview widget, JSON IO; presets.
- Updated documentation: `PRD.md` references maintained; user guide snippets; developer notes.

### 10) Acceptance Criteria (condensed)
- AC-6: Dark theme applies to all UI controls with no layout glitches.
- AC-2/AC-5: Changing tag size updates grid; exported XLSX/DOCX match preview within 1 mm; no clipping; A4 page setup always.
- AC-1: With a valid workbook, preview shows correctly populated tags honoring parsing rules.

### 11) Testing Strategy
- Unit: layout calculation (edge cases for margins/spacing); parsing rules (carry-down, missing headers, concatenation).
- Golden files: compare exported XLSX geometry (column widths/row heights, print area) to expected; DOCX page/item positions.
- UI: theme toggle smoke tests; preview pagination/zoom; template save/load.
- Performance: parse 5k×20 < 3s; preview memory < 300 MB.

### 12) Risks and Mitigations
- DOCX layout fidelity/library choice — Prototype early; abstract via `WordGenerator`; keep fallback minimal XML path.
- Cross-DPI rendering inconsistencies — Use mm as source of truth; single layout engine shared by preview and exports.
- Font availability differences — Prefer standard fonts or bundle where licensing permits.

### 13) Dependencies
- Qt 6 (Widgets), QXlsx (bundled), optional DOCX writer lib.

### 14) Timeline (indicative)
- Stage 1: 1–2 days
- Stage 2: 3–5 days
- Stage 3: 3–5 days
- Stage 4: 4–6 days

### 15) Change Management and Logging (for AI Agents)
- Follow style: `styles/DrunkDogStyle.clang-format` and `.cursor/rules/styles.mdc`.
- Before coding, read: `docs/ProjectTitleByCurrentCR_Implementation.md`, `docs/PRD.md`, `docs/OtherHelpfulFiles/ExcelAPI.txt`.
- After completing each stage or significant subtask, write to:
  - `docs/WorkflowLogs/DevelopmentLog.md`: `[<Timestamp>] <Action> - <Result>`
  - `docs/WorkflowLogs/UserInteractionLog.md`: user-facing confirmations.
- Never mark tasks done without building, running, and verifying ACs above.

### 16) Affected Components
- `src/mainwindow.cpp` (toggle wiring), `include/wordgenerator.h`/`src/wordgenerator.cpp`, `include/excelgenerator.h`/`src/excelgenerator.cpp`, preview/editor widgets, template model and IO.

### 17) Approval
- Approved by: Product/Tech Owner  
Date: 2025-08-08

