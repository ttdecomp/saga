#!/usr/bin/env python3
"""Generate an interactive Canvas/SVG map of matching across the target ELF.

The standalone HTML supports horizontal zooming, panning, unit outlines,
zoom-dependent labels, and function tooltips.

Usage:
    plot_binary_match_map.py [report.json] [--binary res/libTTapp.so]
"""

import argparse
from collections import Counter, defaultdict
import json
import math
import struct
import sys

SHF_ALLOC = 0x2
SHF_EXECINSTR = 0x4
SHF_WRITE = 0x1
SHT_NOBITS = 8
SHT_SYMTAB = 2
STT_FUNC = 2


def _cstring(data, offset):
    end = data.find(b"\0", offset)
    if end == -1:
        end = len(data)
    return data[offset:end].decode("utf-8", errors="replace")


def read_elf32(path):
    """Return sections and symbols from a 32-bit ELF using the standard library."""
    with open(path, "rb") as elf_file:
        data = elf_file.read()

    if data[:4] != b"\x7fELF" or data[4] != 1:
        raise ValueError("expected a 32-bit ELF file")
    byte_order = {1: "<", 2: ">"}.get(data[5])
    if byte_order is None:
        raise ValueError("unsupported ELF byte order")

    header = struct.unpack_from(byte_order + "HHIIIIIHHHHHH", data, 16)
    section_offset = header[5]
    section_entry_size = header[10]
    section_count = header[11]
    section_names_index = header[12]
    section_struct = struct.Struct(byte_order + "IIIIIIIIII")

    if section_entry_size < section_struct.size:
        raise ValueError("invalid ELF section-header size")

    sections = []
    for index in range(section_count):
        offset = section_offset + index * section_entry_size
        fields = section_struct.unpack_from(data, offset)
        sections.append(
            {
                "index": index,
                "name_offset": fields[0],
                "type": fields[1],
                "flags": fields[2],
                "address": fields[3],
                "offset": fields[4],
                "size": fields[5],
                "link": fields[6],
                "info": fields[7],
                "alignment": fields[8],
                "entry_size": fields[9],
            }
        )

    names_section = sections[section_names_index]
    names = data[
        names_section["offset"] : names_section["offset"] + names_section["size"]
    ]
    for section in sections:
        section["name"] = _cstring(names, section["name_offset"])

    symbols = []
    symbol_struct = struct.Struct(byte_order + "IIIBBH")
    for symbol_table in (s for s in sections if s["type"] == SHT_SYMTAB):
        strings_section = sections[symbol_table["link"]]
        strings = data[
            strings_section["offset"] : strings_section["offset"]
            + strings_section["size"]
        ]
        entry_size = symbol_table["entry_size"] or symbol_struct.size
        for offset in range(
            symbol_table["offset"],
            symbol_table["offset"] + symbol_table["size"],
            entry_size,
        ):
            fields = symbol_struct.unpack_from(data, offset)
            section_index = fields[5]
            if not fields[0] or not fields[2] or section_index >= len(sections):
                continue
            symbols.append(
                {
                    "name": _cstring(strings, fields[0]),
                    "address": fields[1],
                    "size": fields[2],
                    "type": fields[3] & 0x0F,
                    "binding": fields[3] >> 4,
                    "section_index": section_index,
                }
            )

    return sections, symbols


def report_function_details(report):
    """Map each function name to its best score, unit, and display name."""
    details = defaultdict(list)
    for unit in report.get("units", []):
        for function in unit.get("functions", []):
            try:
                score = float(function.get("fuzzy_match_percent") or 0.0)
            except (TypeError, ValueError):
                continue
            display_name = function.get("metadata", {}).get(
                "demangled_name", function["name"]
            )
            details[function["name"]].append((score, unit["name"], display_name))
    # A target symbol can be repeated in multiple gonk units. If any unit pairs
    # it successfully, that is the useful score for its unique original range.
    scores = {}
    units = {}
    names = {}
    for name, candidates in details.items():
        score, unit, display_name = max(
            candidates, key=lambda item: (item[0], item[1] != "remaining")
        )
        scores[name] = score
        units[name] = unit
        names[name] = display_name
    return scores, units, names


def build_unit_spans(text_section, functions, units):
    """Return address-ordered unit runs, including inter-function padding."""
    name_counts = Counter(function["name"] for function in functions)
    text_start = text_section["address"]
    text_end = text_start + text_section["size"]
    ranges = []

    for function in functions:
        start = max(function["address"], text_start)
        end = min(function["address"] + function["size"], text_end)
        if end > start:
            unit = None
            if name_counts[function["name"]] == 1:
                unit = units.get(function["name"])
            ranges.append((start, end, unit))

    # Collapse aliases and overlapping symbols before inferring ownership. A
    # cluster is owned only when all known owners agree; unknown aliases do not
    # hide an otherwise unambiguous owner.
    clusters = []
    for start, end, unit in sorted(ranges):
        if clusters and start < clusters[-1][1]:
            cluster_start, cluster_end, owners = clusters[-1]
            clusters[-1] = (
                cluster_start,
                max(cluster_end, end),
                owners | ({unit} if unit is not None else set()),
            )
        else:
            clusters.append((start, end, {unit} if unit is not None else set()))

    spans = []
    current = None
    for start, end, owners in clusters:
        unit = next(iter(owners)) if len(owners) == 1 else None
        if current is not None and unit == current[0]:
            current = (current[0], current[1], max(current[2], end))
            continue

        if current is not None:
            # Attribute padding up to the next function to the preceding run.
            # The outline includes it, while the untouched row fill stays gray.
            spans.append((current[0], current[1], max(current[2], start)))
            current = None
        if unit is not None:
            current = (unit, start, end)

    if current is not None:
        spans.append(current)
    return spans


def build_payload(
    sections,
    text_section,
    functions,
    scores,
    units,
    names,
    unit_spans,
    row_bytes,
    cell_bytes,
):
    """Create compact JSON-ready data for the browser visualization."""
    allocated = [
        section
        for section in sections
        if section["flags"] & SHF_ALLOC and section["size"]
    ]
    unit_names = sorted(set(units.values()))
    unit_ids = {name: index for index, name in enumerate(unit_names)}
    name_counts = Counter(function["name"] for function in functions)
    text_start = text_section["address"]
    text_end = text_start + text_section["size"]

    function_data = []
    for function in functions:
        start = max(function["address"], text_start)
        end = min(function["address"] + function["size"], text_end)
        if end <= start:
            continue
        raw_name = function["name"]
        ambiguous = name_counts[raw_name] > 1
        unit = units.get(raw_name)
        function_data.append(
            {
                "a": start - text_start,
                "s": end - start,
                "n": names.get(raw_name, raw_name),
                "r": raw_name,
                "u": -1 if ambiguous else unit_ids.get(unit, -1),
                "p": None if raw_name not in scores else round(scores[raw_name], 4),
                "x": ambiguous,
            }
        )

    section_data = []
    for section in allocated:
        if section["flags"] & SHF_EXECINSTR:
            kind = "exec"
        elif section["type"] == SHT_NOBITS:
            kind = "nobits"
        elif section["flags"] & SHF_WRITE:
            kind = "write"
        else:
            kind = "read"
        section_data.append(
            {
                "n": section["name"],
                "a": section["address"],
                "s": section["size"],
                "k": kind,
            }
        )

    span_data = [
        {"a": start - text_start, "s": end - start, "u": unit_ids[unit]}
        for unit, start, end in unit_spans
    ]
    return {
        "sections": section_data,
        "textStart": text_start,
        "textSize": text_section["size"],
        "rowBytes": row_bytes,
        "cellBytes": cell_bytes,
        "rows": math.ceil(text_section["size"] / row_bytes),
        "units": unit_names,
        "functions": function_data,
        "spans": span_data,
    }


HTML_TEMPLATE = r"""<!doctype html>
<html lang="en" data-theme="dark">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<meta name="darkreader-lock">
<title>Original binary address match map</title>
<script>document.documentElement.dataset.theme = matchMedia("(prefers-color-scheme: dark)").matches ? "dark" : "light";</script>
<script src="https://cdn.jsdelivr.net/npm/@tailwindcss/browser@4"></script>
<script src="https://cdn.jsdelivr.net/npm/d3@7.9.0/dist/d3.min.js"></script>
<style>
  html[data-theme="light"] {
    --surface: #ffffff;
    --surface-alt: #e7edf4;
    --text: #0b1320;
    --muted: #475569;
    --ui-border: #94a3b8;
    --ui-border-strong: #526173;
    --accent: #175cd3;
    --accent-text: #ffffff;
    --row: #d7dee7;
  }
  html[data-theme="dark"] {
    --surface: #151b23;
    --surface-alt: #080d14;
    --text: #f2f5f8;
    --muted: #b6c2d1;
    --ui-border: #526173;
    --ui-border-strong: #94a3b8;
    --accent: #69b4ff;
    --accent-text: #07131f;
    --row: #343e49;
  }
  :root {
    --page: var(--surface);
    --map-border: var(--ui-border);
    --outline: var(--text);
    --tooltip: var(--surface-alt);
    --tooltip-text: var(--text);
    --mono: ui-monospace, SFMono-Regular, Menlo, Monaco, Consolas,
            "Liberation Mono", monospace;
  }
  .map-layer { position: absolute; inset: 0; display: block; width: 100%;
               height: 100%; user-select: none; }
  #map-canvas { pointer-events: none; }
  #binary-map { touch-action: none; pointer-events: none; }
  .interaction { fill: transparent; pointer-events: all; }
  .axis-label, .row-label, .section-label, .unit-label, .function-label {
    fill: var(--text);
  }
  .row-label, .axis-label { font-size: 11px; }
  .section-label { font-size: 10px; pointer-events: none; }
  .unit-label, .function-label { font-size: 11px; font-weight: 500;
                                pointer-events: none; paint-order: stroke fill;
                                stroke: var(--page); stroke-width: 1.5px;
                                stroke-linejoin: round; }
  .unit-label { font-weight: 600; }
  .address-label, .row-label, .section-label, .unit-label, .function-label,
  .tree-name, .map-tooltip {
    font-family: var(--mono);
  }
  .row-frame { fill: var(--row); stroke: var(--page); stroke-width: 1; }
  .viewport-track { fill: var(--row); stroke: var(--map-border); stroke-width: .6; }
  .viewport-window { fill: color-mix(in oklab, var(--accent) 40%, transparent);
                     stroke: var(--accent); stroke-width: 1; }
  .hover-outline { fill: none; stroke: var(--outline); stroke-width: 1.5;
                   pointer-events: none; }
  .map-tooltip { position: fixed; display: none; pointer-events: none;
                 z-index: 50; max-width: min(520px, calc(100vw - 24px));
                 padding: 9px 11px; background: var(--tooltip);
                 color: var(--tooltip-text); border: 1px solid var(--map-border);
                 border-radius: 8px; font-size: 12px;
                 box-shadow: 0 12px 30px rgb(0 0 0 / .28); }
  .map-tooltip strong { display: block; margin-bottom: 4px; font-weight: 500;
                        overflow-wrap: anywhere; }
  .tooltip-grid { display: grid; grid-template-columns: max-content minmax(0, 1fr);
                  column-gap: 12px; row-gap: 2px; }
  .tooltip-label { color: var(--muted); text-align: right; }
  .tooltip-label::after { content: ":"; }
  .tooltip-value { min-width: 0; color: var(--tooltip-text);
                   overflow-wrap: anywhere; }
  .match-gradient { background: linear-gradient(90deg, #a50026 0%,
                      #fdae61 25%, #ffffbf 50%, #a6d96a 75%, #006837 100%); }
</style>
</head>
<body class="h-screen overflow-hidden bg-[var(--surface-alt)] text-[var(--text)]">
<main class="mx-auto flex h-full max-w-[1900px] flex-col gap-2 p-3 sm:p-4">
  <div class="flex shrink-0 flex-wrap items-center gap-2">
    <h1 class="mr-2 text-lg font-semibold sm:text-xl">Original binary address match map</h1>
  </div>
  <div class="flex min-h-0 flex-1 gap-3 sm:gap-4">
    <section class="flex min-w-0 flex-1 flex-col" aria-label="Binary address map">
      <div class="flex min-h-0 flex-1 flex-col overflow-hidden rounded-lg border border-[var(--ui-border)] bg-[var(--surface)] shadow-sm">
        <div class="flex h-9 shrink-0 items-end gap-1 border-b border-[var(--ui-border)] px-2" role="tablist" aria-label="Map views">
          <button id="address-tab" class="h-9 border-b-2 border-transparent px-3 font-mono text-xs text-[var(--muted)] transition-colors hover:text-[var(--text)] aria-selected:border-[var(--accent)] aria-selected:text-[var(--text)]" type="button" role="tab" aria-selected="true" aria-controls="address-panel">Address map</button>
          <button id="treemap-tab" class="h-9 border-b-2 border-transparent px-3 font-mono text-xs text-[var(--muted)] transition-colors hover:text-[var(--text)] aria-selected:border-[var(--accent)] aria-selected:text-[var(--text)]" type="button" role="tab" aria-selected="false" aria-controls="treemap-panel">Function sizes</button>
        </div>
        <div id="address-panel" class="flex min-h-0 flex-1 flex-col" role="tabpanel" aria-labelledby="address-tab">
          <div id="map-stack" class="relative min-h-0 flex-1 overflow-hidden">
            <svg id="map-base" class="map-layer" aria-hidden="true"></svg>
            <canvas id="map-canvas" class="map-layer" aria-hidden="true"></canvas>
            <svg id="binary-map" class="map-layer" role="img"
                 aria-labelledby="map-title map-desc">
              <title id="map-title">Original binary matching by virtual address</title>
              <desc id="map-desc">Allocated ELF sections and a folded text-section map colored by function matching percentage, with unit outlines.</desc>
            </svg>
            <div id="map-controls" class="pointer-events-auto absolute right-[1.5%] z-20 flex h-5 items-center gap-1.5 text-[11px]">
              <span class="font-medium">Horizontally zoomable</span>
              <span class="hidden text-[var(--muted)] min-[1500px]:inline">Wheel/pinch: zoom · drag: pan · hover: details</span>
              <button id="reset" class="inline-flex h-5 items-center rounded border border-[var(--ui-border-strong)] px-1.5 text-[10px] font-medium transition-colors hover:bg-[var(--surface-alt)] focus-visible:outline-2 focus-visible:outline-offset-1 focus-visible:outline-[var(--accent)]" type="button">Reset</button>
              <span id="zoom-status" class="inline-flex h-5 items-center rounded border border-[var(--ui-border-strong)] px-1.5 text-[10px] tabular-nums" aria-live="polite">Zoom 1×</span>
            </div>
          </div>
        </div>
        <div id="treemap-panel" class="relative hidden min-h-0 flex-1 overflow-hidden" role="tabpanel" aria-labelledby="treemap-tab">
          <canvas id="function-treemap" class="absolute inset-0 h-full w-full" role="img" aria-label="Treemap of all text-section functions, with rectangle area proportional to code size"></canvas>
          <div id="treemap-hover" class="pointer-events-none absolute hidden border-2 border-[var(--text)]"></div>
        </div>
        <div class="flex shrink-0 flex-wrap gap-x-3 gap-y-1 border-t border-[var(--ui-border)] px-3 py-2 text-xs text-[var(--muted)]" aria-label="Map legend">
          <span class="inline-flex items-center gap-1"><span class="match-gradient inline-block h-2.5 w-20 shrink-0 rounded-sm"></span>0–100% function match</span>
          <span class="inline-flex items-center gap-1"><span class="inline-block h-2.5 w-4 shrink-0 bg-[#7651a8]"></span>Ambiguous duplicate</span>
          <span class="inline-flex items-center gap-1"><span class="inline-block h-2.5 w-4 shrink-0 bg-[var(--row)]"></span>No scored function</span>
          <span class="inline-flex items-center gap-1"><span class="inline-block h-2.5 w-4 shrink-0 border border-[var(--text)]"></span>Unit boundary</span>
        </div>
      </div>
    </section>
    <aside class="flex h-full min-h-0 w-56 shrink-0 flex-col overflow-hidden rounded-lg border border-[var(--ui-border)] bg-[var(--surface)] shadow-sm sm:w-64 lg:w-72" aria-labelledby="unit-sidebar-title">
      <div class="border-b border-[var(--ui-border)] p-3">
        <h2 id="unit-sidebar-title" class="mb-2 text-sm font-semibold">Units</h2>
        <div class="flex items-center gap-1.5">
          <button id="show-all-units" class="inline-flex h-7 items-center rounded border border-[var(--ui-border-strong)] px-2 text-xs font-medium transition-colors hover:bg-[var(--surface-alt)] focus-visible:outline-2 focus-visible:outline-offset-1 focus-visible:outline-[var(--accent)]" type="button">All</button>
          <button id="hide-all-units" class="inline-flex h-7 items-center rounded border border-[var(--ui-border-strong)] px-2 text-xs font-medium transition-colors hover:bg-[var(--surface-alt)] focus-visible:outline-2 focus-visible:outline-offset-1 focus-visible:outline-[var(--accent)]" type="button">None</button>
          <span id="unit-summary" class="ml-auto inline-flex h-5 items-center rounded bg-[var(--surface-alt)] px-1.5 text-[10px] tabular-nums"></span>
        </div>
      </div>
      <div id="unit-tree" class="min-h-0 flex-1 overflow-auto p-2 text-xs"></div>
    </aside>
  </div>
</main>
<div id="tooltip" class="map-tooltip" role="tooltip"></div>
<script>
const data = /*__DATA__*/;
const unitVisibility = new Uint8Array(data.units.length);
unitVisibility.fill(1);
const base = d3.select("#map-base");
const svg = d3.select("#binary-map");
const mapStack = d3.select("#map-stack");
const mapControls = document.getElementById("map-controls");
const addressTab = document.getElementById("address-tab");
const treemapTab = document.getElementById("treemap-tab");
const addressPanel = document.getElementById("address-panel");
const treemapPanel = document.getElementById("treemap-panel");
const canvas = document.getElementById("map-canvas");
const context = canvas.getContext("2d");
const gestureCanvas = document.createElement("canvas");
const gestureContext = gestureCanvas.getContext("2d");
const treemapCanvas = document.getElementById("function-treemap");
const treemapContext = treemapCanvas.getContext("2d");
const treemapHover = document.getElementById("treemap-hover");
const tooltip = document.getElementById("tooltip");
const width = 1200;
const left = 118;
const right = 18;
const plotWidth = width - left - right;
const sectionY = 34;
const sectionHeight = 48;
const bitmapY = 126;
const minimumRowHeight = 22;
const minimumRowBytes = 0x10 * 1024;
const bottomPadding = 36;
let rowBytes = data.rowBytes;
let rowCount = data.rows;
let rowHeight = minimumRowHeight;
let bitmapHeight = rowCount * rowHeight;
let totalHeight = bitmapY + bitmapHeight + bottomPadding;

function quantizeRowBytes(requiredBytes) {
  let result = minimumRowBytes;
  while (result < requiredBytes) result *= 2;
  return result;
}

function fitRowsToViewport() {
  const bounds = mapStack.node().getBoundingClientRect();
  if (!bounds.width || !bounds.height) return false;
  const fittedHeight = Math.max(bitmapY + minimumRowHeight + bottomPadding,
    width * bounds.height / bounds.width);
  const maximumRows = Math.max(1,
    Math.floor((fittedHeight - bitmapY - bottomPadding) / minimumRowHeight));
  const fittedRowBytes = quantizeRowBytes(data.textSize / maximumRows);
  const fittedRowCount = Math.ceil(data.textSize / fittedRowBytes);
  const fittedRowHeight =
    (fittedHeight - bitmapY - bottomPadding) / fittedRowCount;
  const changed = fittedRowBytes !== rowBytes || fittedRowCount !== rowCount ||
    Math.abs(fittedRowHeight - rowHeight) > .05 ||
    Math.abs(fittedHeight - totalHeight) > .5;
  rowBytes = fittedRowBytes;
  rowCount = fittedRowCount;
  rowHeight = fittedRowHeight;
  bitmapHeight = rowCount * rowHeight;
  totalHeight = fittedHeight;
  return changed;
}

function positionMapControls() {
  mapControls.style.top = `${(bitmapY - 29) * 100 / totalHeight}%`;
  mapControls.style.transform = "translateY(-50%)";
}

fitRowsToViewport();
positionMapControls();
[base, svg].forEach(layer => layer.attr("viewBox", `0 0 ${width} ${totalHeight}`));

const defs = svg.append("defs");
const bitmapClip = defs.append("clipPath").attr("id", "bitmap-clip")
  .append("rect").attr("x", left).attr("y", bitmapY)
  .attr("width", plotWidth).attr("height", bitmapHeight);

const sectionStart = d3.min(data.sections, d => d.a);
const sectionEnd = d3.max(data.sections, d => d.a + d.s);
const sectionScale = d3.scaleLinear().domain([sectionStart, sectionEnd])
  .range([left, left + plotWidth]);
const sectionColors = {exec: "#4c78a8", read: "#72b7b2",
                       write: "#f58518", nobits: "#9d9da1"};
base.append("text").attr("class", "axis-label").attr("x", left)
  .attr("y", 20).text("Allocated ELF sections by virtual address");
base.append("g").selectAll("rect").data(data.sections).join("rect")
  .attr("x", d => sectionScale(d.a)).attr("y", sectionY)
  .attr("width", d => Math.max(.5, sectionScale(d.a + d.s) - sectionScale(d.a)))
  .attr("height", sectionHeight).attr("fill", d => sectionColors[d.k])
  .attr("stroke", "var(--page)").attr("stroke-width", .7);
base.append("g").selectAll("text").data(data.sections.filter(d =>
  sectionScale(d.a + d.s) - sectionScale(d.a) > 38)).join("text")
  .attr("class", "section-label").attr("text-anchor", "middle")
  .attr("x", d => (sectionScale(d.a) + sectionScale(d.a + d.s)) / 2)
  .attr("y", sectionY + sectionHeight / 2 + 4).text(d => d.n);
base.append("text").attr("class", "axis-label address-label").attr("x", left)
  .attr("y", sectionY + sectionHeight + 16)
  .text(`0x${sectionStart.toString(16)} → 0x${sectionEnd.toString(16)}`);

const layoutLabel = base.append("text")
  .attr("class", "axis-label address-label").attr("x", left)
  .attr("y", bitmapY - 15)
  .text(`.text · ${Math.round(rowBytes / 1024)} KiB per row`);
base.append("rect").attr("class", "viewport-track")
  .attr("x", left).attr("y", bitmapY - 9)
  .attr("width", plotWidth).attr("height", 5).attr("rx", 2.5);
const viewportIndicator = svg.append("rect").attr("class", "viewport-window")
  .attr("x", left).attr("y", bitmapY - 9)
  .attr("width", plotWidth).attr("height", 5).attr("rx", 2.5);
const hoverOutline = svg.append("rect").attr("class", "hover-outline")
  .attr("clip-path", "url(#bitmap-clip)").attr("display", "none");

const rows = base.append("g");
function renderRows() {
  rows.selectAll("rect").data(d3.range(rowCount)).join("rect")
    .attr("class", "row-frame").attr("x", left)
    .attr("y", d => bitmapY + d * rowHeight)
    .attr("width", plotWidth).attr("height", rowHeight);
  rows.selectAll("text").data(d3.range(rowCount)).join("text")
    .attr("class", "row-label").attr("text-anchor", "end")
    .attr("x", left - 8).attr("y", d => bitmapY + d * rowHeight + 15)
    .text(d => `0x${(data.textStart + d * rowBytes).toString(16).padStart(8, "0")}`);
}
renderRows();

let xScale = d3.scaleLinear().domain([0, rowBytes]).range([0, plotWidth]);
function splitRange(address, size, extra) {
  const result = [];
  let cursor = address;
  const end = Math.min(address + size, data.textSize);
  while (cursor < end) {
    const row = Math.floor(cursor / rowBytes);
    const rowEnd = (row + 1) * rowBytes;
    const chunkEnd = Math.min(end, rowEnd);
    const localStart = cursor - row * rowBytes;
    result.push({...extra, row, x: xScale(localStart),
                 w: Math.max(.12, xScale(chunkEnd - row * rowBytes) - xScale(localStart))});
    cursor = chunkEnd;
  }
  return result;
}

let functionSegments = [];
let outlineSegments = [];

function fillColor(f) {
  if (f.x) return "#7651a8";
  if (f.p === null) return "var(--row)";
  return d3.interpolateRdYlGn(f.p / 100);
}
function unitIsVisible(unit) { return unit < 0 || unitVisibility[unit] === 1; }
function functionIsVisible(f) { return unitIsVisible(f.u); }
function unitName(f) { return f.u < 0 ? "(unassigned)" : data.units[f.u]; }
function cleanUnitName(name) {
  return name.endsWith(".o") ? name.slice(0, -2) : name;
}
function shortUnitName(name) {
  name = cleanUnitName(name).split("/").pop();
  return name.endsWith(".o") ? name.slice(0, -2) : name;
}

function indexByRow(segments) {
  const indexes = Array.from({length: rowCount}, () => ({items: [], ends: []}));
  segments.forEach(segment => indexes[segment.row].items.push(segment));
  indexes.forEach(index => {
    index.items.sort((a, b) => a.x - b.x || a.w - b.w);
    let farthestEnd = 0;
    index.ends = index.items.map(segment => {
      farthestEnd = Math.max(farthestEnd, segment.x + segment.w);
      return farthestEnd;
    });
  });
  return indexes;
}

let functionRows = [];
let outlineRows = [];

function visibleSegments(index, minimum, maximum) {
  let low = 0;
  let high = index.ends.length;
  while (low < high) {
    const middle = (low + high) >> 1;
    if (index.ends[middle] <= minimum) low = middle + 1;
    else high = middle;
  }
  const result = [];
  for (let i = low; i < index.items.length; i += 1) {
    const segment = index.items[i];
    if (segment.x >= maximum) break;
    if (segment.x + segment.w > minimum) result.push(segment);
  }
  return result;
}

function segmentAt(index, x, predicate = () => true) {
  let low = 0;
  let high = index.items.length;
  while (low < high) {
    const middle = (low + high) >> 1;
    if (index.items[middle].x <= x) low = middle + 1;
    else high = middle;
  }
  for (let i = low - 1; i >= 0 && index.ends[i] > x; i -= 1) {
    const segment = index.items[i];
    if (segment.x <= x && x < segment.x + segment.w && predicate(segment)) {
      return segment;
    }
  }
  return null;
}

function buildOverviewRows() {
  const totalCells = Math.ceil(data.textSize / data.cellBytes);
  const columns = Math.ceil(rowBytes / data.cellBytes);
  const scoreWeight = new Float64Array(totalCells);
  const scoredBytes = new Float64Array(totalCells);
  const ambiguousBytes = new Float64Array(totalCells);
  data.functions.forEach(f => {
    if (!functionIsVisible(f)) return;
    let cursor = f.a;
    const end = Math.min(f.a + f.s, data.textSize);
    while (cursor < end) {
      const cell = Math.floor(cursor / data.cellBytes);
      const chunkEnd = Math.min(end, (cell + 1) * data.cellBytes);
      const bytes = chunkEnd - cursor;
      if (f.x) ambiguousBytes[cell] += bytes;
      else if (f.p !== null) {
        scoredBytes[cell] += bytes;
        scoreWeight[cell] += bytes * f.p;
      }
      cursor = chunkEnd;
    }
  });

  const overview = Array.from({length: rowCount}, () => []);
  const cellWidth = xScale(data.cellBytes);
  for (let cell = 0; cell < totalCells; cell += 1) {
    if (!scoredBytes[cell] && !ambiguousBytes[cell]) continue;
    const row = Math.floor(cell / columns);
    const column = cell % columns;
    const bytesInCell = Math.min(data.cellBytes,
      data.textSize - cell * data.cellBytes);
    overview[row].push({
      x: column * cellWidth,
      w: xScale(bytesInCell),
      p: scoredBytes[cell] ? scoreWeight[cell] / scoredBytes[cell] : null,
      scored: Math.min(1, scoredBytes[cell] / bytesInCell),
      ambiguous: Math.min(1, ambiguousBytes[cell] / bytesInCell),
    });
  }
  return overview;
}

let overviewRows = [];
function rebuildSegmentLayout() {
  functionSegments = [];
  data.functions.forEach((f, index) => splitRange(f.a, f.s, {f, index})
    .forEach((segment, part) =>
      functionSegments.push({...segment, key: `${index}:${part}`})));
  outlineSegments = [];
  data.spans.forEach((span, index) => splitRange(span.a, span.s, {span, index})
    .forEach((segment, part) =>
      outlineSegments.push({...segment, key: `${index}:${part}`})));
  outlineSegments.forEach(segment => {
    segment.fullName = cleanUnitName(data.units[segment.span.u]);
    segment.shortName = shortUnitName(segment.fullName);
  });
  functionRows = indexByRow(functionSegments);
  outlineRows = indexByRow(outlineSegments);
  overviewRows = buildOverviewRows();
}
rebuildSegmentLayout();
const detailZoom = 8;
const colorScheme = matchMedia("(prefers-color-scheme: dark)");
let currentTransform = d3.zoomIdentity;
let currentLocalX = 0;
let hoveredSegment = null;
let lastPointer = null;
let tooltipBox = {width: 0, height: 0};
let zooming = false;
let drawScheduled = false;
let drawFrame = 0;
let labelPositionScheduled = false;
let gestureStartScale = 1;
let gestureStartLocalX = 0;

function populateTooltip(event, titleText, lines) {
  tooltip.replaceChildren();
  const title = document.createElement("strong");
  title.textContent = titleText;
  tooltip.append(title);
  const grid = document.createElement("div");
  grid.className = "tooltip-grid";
  lines.forEach(({label, value}) => {
    const labelElement = document.createElement("span");
    labelElement.className = "tooltip-label";
    labelElement.textContent = label;
    const valueElement = document.createElement("span");
    valueElement.className = "tooltip-value";
    valueElement.textContent = value;
    grid.append(labelElement, valueElement);
  });
  tooltip.append(grid);
  tooltip.style.display = "block";
  tooltipBox = tooltip.getBoundingClientRect();
  moveTooltip(event);
}

function showTooltip(event, segment) {
  const f = segment.f;
  populateTooltip(event, f.n, [
    {label: "Unit", value: unitName(f)},
    {label: "Address", value: `0x${(data.textStart + f.a).toString(16)}–0x${
      (data.textStart + f.a + f.s).toString(16)}`},
    {label: "Size", value: `${f.s.toLocaleString()} bytes`},
    {label: "Match", value: f.x ? "ambiguous duplicate name" :
      (f.p === null ? "not scored" : `${f.p.toFixed(2)}%`)},
    {label: "Symbol", value: f.r}
  ]);
}
function moveTooltip(event) {
  const pad = 12;
  tooltip.style.left = `${Math.min(event.clientX + 14, innerWidth - tooltipBox.width - pad)}px`;
  tooltip.style.top = `${Math.min(event.clientY + 14, innerHeight - tooltipBox.height - pad)}px`;
}
function hideTooltip() {
  tooltip.style.display = "none";
  if (hoveredSegment !== null) {
    hoveredSegment = null;
    updateHoverOutline();
  }
}

function visibleRange(transform, localX) {
  return [Math.max(0, -localX / transform.k),
          Math.min(plotWidth, (plotWidth - localX) / transform.k)];
}

function updateViewportIndicator(transform, localX) {
  const [minimum, maximum] = visibleRange(transform, localX);
  const indicatorWidth = Math.max(2, maximum - minimum);
  const indicatorX = Math.min(minimum, plotWidth - indicatorWidth);
  viewportIndicator.attr("x", left + indicatorX)
    .attr("width", indicatorWidth);
}

function screenX(x) { return left + currentLocalX + x * currentTransform.k; }

function updateHoverOutline() {
  if (hoveredSegment === null) {
    hoverOutline.attr("display", "none");
    return;
  }
  hoverOutline.attr("display", null)
    .attr("x", screenX(hoveredSegment.x))
    .attr("y", bitmapY + hoveredSegment.row * rowHeight)
    .attr("width", Math.max(.12,
      hoveredSegment.w * currentTransform.k))
    .attr("height", rowHeight);
}

function drawOverview(minimum, maximum) {
  overviewRows.forEach((cells, row) => {
    const y = bitmapY + row * rowHeight;
    cells.forEach(cell => {
      if (cell.x >= maximum || cell.x + cell.w <= minimum) return;
      const x = screenX(cell.x);
      const cellWidth = Math.max(.5, cell.w * currentTransform.k + .25);
      if (cell.scored) {
        context.globalAlpha = cell.scored;
        context.fillStyle = d3.interpolateRdYlGn(cell.p / 100);
        context.fillRect(x, y, cellWidth, rowHeight);
      }
      if (cell.ambiguous) {
        context.globalAlpha = cell.ambiguous;
        context.fillStyle = "#7651a8";
        context.fillRect(x, y, cellWidth, rowHeight);
      }
    });
  });
  context.globalAlpha = 1;
}

function drawFunctions(minimum, maximum) {
  functionRows.forEach((index, row) => {
    const y = bitmapY + row * rowHeight;
    visibleSegments(index, minimum, maximum).forEach(segment => {
      if (!functionIsVisible(segment.f)) return;
      if (!segment.f.x && segment.f.p === null) return;
      context.fillStyle = fillColor(segment.f);
      context.fillRect(screenX(segment.x), y,
        Math.max(.12, segment.w * currentTransform.k), rowHeight);
    });
  });
}

function drawOutlines(minimum, maximum) {
  context.beginPath();
  outlineRows.forEach((index, row) => {
    visibleSegments(index, minimum, maximum).forEach(segment => {
      if (!unitIsVisible(segment.span.u)) return;
      context.rect(screenX(segment.x), bitmapY + row * rowHeight,
        Math.max(.12, segment.w * currentTransform.k), rowHeight);
    });
  });
  context.globalAlpha = 1;
  context.strokeStyle = colorScheme.matches ? "#f2f2f2" : "#171717";
  context.lineWidth = .8;
  context.stroke();
}

function resizeCanvas() {
  const bounds = base.node().getBoundingClientRect();
  if (!bounds.width || !bounds.height) return;
  const pixelRatio = Math.min(devicePixelRatio || 1, 2);
  const canvasWidth = Math.max(1, Math.round(bounds.width * pixelRatio));
  const canvasHeight = Math.max(1, Math.round(bounds.height * pixelRatio));
  if (canvas.width !== canvasWidth || canvas.height !== canvasHeight) {
    canvas.width = canvasWidth;
    canvas.height = canvasHeight;
  }
}

function drawMap() {
  resizeCanvas();
  context.setTransform(1, 0, 0, 1, 0, 0);
  context.clearRect(0, 0, canvas.width, canvas.height);
  context.setTransform(canvas.width / width, 0, 0,
                       canvas.height / totalHeight, 0, 0);
  context.imageSmoothingEnabled = false;
  context.save();
  context.beginPath();
  context.rect(left, bitmapY, plotWidth, bitmapHeight);
  context.clip();
  const [minimum, maximum] = visibleRange(currentTransform, currentLocalX);
  if (currentTransform.k < detailZoom) drawOverview(minimum, maximum);
  else drawFunctions(minimum, maximum);
  drawOutlines(minimum, maximum);
  context.restore();
}

function scheduleDraw() {
  if (zooming || drawScheduled) return;
  drawScheduled = true;
  drawFrame = requestAnimationFrame(() => {
    drawScheduled = false;
    drawFrame = 0;
    drawMap();
  });
}

function cancelScheduledDraw() {
  if (drawFrame) cancelAnimationFrame(drawFrame);
  drawScheduled = false;
  drawFrame = 0;
}

function captureGestureCanvas() {
  if (gestureCanvas.width !== canvas.width ||
      gestureCanvas.height !== canvas.height) {
    gestureCanvas.width = canvas.width;
    gestureCanvas.height = canvas.height;
  }
  gestureContext.setTransform(1, 0, 0, 1, 0, 0);
  gestureContext.clearRect(0, 0, gestureCanvas.width, gestureCanvas.height);
  gestureContext.drawImage(canvas, 0, 0);
}

function drawGestureCanvas() {
  const scaleRatio = currentTransform.k / gestureStartScale;
  const offset = left + currentLocalX -
    scaleRatio * (left + gestureStartLocalX);
  context.setTransform(1, 0, 0, 1, 0, 0);
  context.clearRect(0, 0, canvas.width, canvas.height);
  context.setTransform(canvas.width / width, 0, 0,
                       canvas.height / totalHeight, 0, 0);
  context.imageSmoothingEnabled = false;
  context.save();
  context.beginPath();
  context.rect(left, bitmapY, plotWidth, bitmapHeight);
  context.clip();
  context.translate(offset, 0);
  context.scale(scaleRatio, 1);
  context.drawImage(gestureCanvas,
    0, 0, gestureCanvas.width, gestureCanvas.height,
    0, 0, width, totalHeight);
  context.restore();
}

const labelLayer = svg.append("g").attr("clip-path", "url(#bitmap-clip)")
  .append("g").attr("transform", `translate(${left},${bitmapY})`);
let renderedLabelScale = 1;
let renderedLabelLocalX = 0;
const labelCharacterWidth = 6.65;
const labelHorizontalPadding = 8;
const truncatedLabelCharacters = 3;

function labelFits(name, width) {
  return width >= name.length * labelCharacterWidth + labelHorizontalPadding;
}

function fittedLabelText(name, width) {
  if (labelFits(name, width)) return name;
  const characters = Math.floor(
    (width - labelHorizontalPadding) / labelCharacterWidth);
  if (characters < truncatedLabelCharacters + 1) return null;
  return `${name.slice(0, truncatedLabelCharacters)}…`;
}

function fittedUnitLabelText(segment, width) {
  if (labelFits(segment.fullName, width)) return segment.fullName;
  return fittedLabelText(segment.shortName, width);
}

let activeView = "address";
let treemapLeaves = [];
let treemapBins = [];
const treemapBinColumns = 64;
const treemapBinRows = 64;
let hoveredTreemapLeaf = null;
let treemapScheduled = false;

function resolveThemeColor(name) {
  return getComputedStyle(document.documentElement)
    .getPropertyValue(name).trim();
}

function treemapFill(f) {
  if (f.x) return "#7651a8";
  if (f.p === null) return resolveThemeColor("--row");
  return d3.interpolateRdYlGn(f.p / 100);
}

function indexTreemapLeaves(width, height) {
  treemapBins = Array.from(
    {length: treemapBinColumns * treemapBinRows}, () => []);
  treemapLeaves.forEach(leaf => {
    const x0 = Math.max(0, Math.min(treemapBinColumns - 1,
      Math.floor(leaf.x0 / width * treemapBinColumns)));
    const x1 = Math.max(0, Math.min(treemapBinColumns - 1,
      Math.floor(Math.max(leaf.x0, leaf.x1 - .001) /
        width * treemapBinColumns)));
    const y0 = Math.max(0, Math.min(treemapBinRows - 1,
      Math.floor(leaf.y0 / height * treemapBinRows)));
    const y1 = Math.max(0, Math.min(treemapBinRows - 1,
      Math.floor(Math.max(leaf.y0, leaf.y1 - .001) /
        height * treemapBinRows)));
    for (let y = y0; y <= y1; y += 1) {
      for (let x = x0; x <= x1; x += 1) {
        treemapBins[y * treemapBinColumns + x].push(leaf);
      }
    }
  });
}

function renderTreemap() {
  if (activeView !== "treemap") return;
  const bounds = treemapPanel.getBoundingClientRect();
  if (!bounds.width || !bounds.height) return;
  const pixelRatio = Math.min(devicePixelRatio || 1, 2);
  const canvasWidth = Math.max(1, Math.round(bounds.width * pixelRatio));
  const canvasHeight = Math.max(1, Math.round(bounds.height * pixelRatio));
  if (treemapCanvas.width !== canvasWidth ||
      treemapCanvas.height !== canvasHeight) {
    treemapCanvas.width = canvasWidth;
    treemapCanvas.height = canvasHeight;
  }
  treemapContext.setTransform(pixelRatio, 0, 0, pixelRatio, 0, 0);
  treemapContext.clearRect(0, 0, bounds.width, bounds.height);

  const visibleFunctions = data.functions.filter(functionIsVisible);
  if (visibleFunctions.length === 0) {
    treemapLeaves = [];
    treemapBins = Array.from(
      {length: treemapBinColumns * treemapBinRows}, () => []);
    treemapCanvas.setAttribute("aria-label",
      "Function-size treemap with no visible functions");
    treemapContext.fillStyle = resolveThemeColor("--muted");
    treemapContext.font = `500 11px ${resolveThemeColor("--mono")}`;
    treemapContext.textAlign = "center";
    treemapContext.textBaseline = "middle";
    treemapContext.fillText("No visible functions",
      bounds.width / 2, bounds.height / 2);
    hoveredTreemapLeaf = null;
    treemapHover.classList.add("hidden");
    hideTooltip();
    return;
  }
  const grouped = d3.group(visibleFunctions, unitName);
  const hierarchy = {
    children: [...grouped].map(([unit, functions]) => ({
      unit,
      children: functions.map(f => ({function: f}))
    }))
  };
  const root = d3.hierarchy(hierarchy)
    .sum(node => node.function ? node.function.s : 0)
    .sort((a, b) => b.value - a.value);
  d3.treemap().tile(d3.treemapSquarify)
    .size([bounds.width, bounds.height])
    .paddingOuter(2).paddingInner(.7).round(true)(root);
  treemapLeaves = root.leaves();
  indexTreemapLeaves(bounds.width, bounds.height);
  treemapCanvas.setAttribute("aria-label",
    `Function-size treemap showing ${treemapLeaves.length.toLocaleString()} ` +
    "visible text-section functions; rectangle area represents code size");

  const textColor = resolveThemeColor("--text");
  const haloColor = resolveThemeColor("--page");
  const boundaryColor = resolveThemeColor("--ui-border-strong");
  const mono = resolveThemeColor("--mono");
  treemapContext.font = `500 11px ${mono}`;
  treemapContext.textAlign = "center";
  treemapContext.textBaseline = "middle";
  treemapContext.lineJoin = "round";
  treemapLeaves.forEach(leaf => {
    const f = leaf.data.function;
    const cellWidth = Math.max(0, leaf.x1 - leaf.x0);
    const cellHeight = Math.max(0, leaf.y1 - leaf.y0);
    treemapContext.fillStyle = treemapFill(f);
    treemapContext.fillRect(leaf.x0, leaf.y0, cellWidth, cellHeight);
    if (cellHeight < 13) return;
    const label = fittedLabelText(f.n, cellWidth);
    if (label === null) return;
    const x = (leaf.x0 + leaf.x1) / 2;
    const y = (leaf.y0 + leaf.y1) / 2;
    treemapContext.strokeStyle = haloColor;
    treemapContext.lineWidth = 2.5;
    treemapContext.strokeText(label, x, y, Math.max(0, cellWidth - 4));
    treemapContext.fillStyle = textColor;
    treemapContext.fillText(label, x, y, Math.max(0, cellWidth - 4));
  });
  treemapContext.strokeStyle = boundaryColor;
  treemapContext.lineWidth = 1;
  root.children?.forEach(unit => {
    treemapContext.strokeRect(unit.x0 + .5, unit.y0 + .5,
      Math.max(0, unit.x1 - unit.x0 - 1),
      Math.max(0, unit.y1 - unit.y0 - 1));
  });

  if (hoveredTreemapLeaf !== null &&
      !treemapLeaves.includes(hoveredTreemapLeaf)) {
    hoveredTreemapLeaf = null;
    treemapHover.classList.add("hidden");
    hideTooltip();
  }
}

function scheduleTreemapRender() {
  if (treemapScheduled) return;
  treemapScheduled = true;
  requestAnimationFrame(() => {
    treemapScheduled = false;
    renderTreemap();
  });
}

function treemapLeafAt(event) {
  const bounds = treemapPanel.getBoundingClientRect();
  const x = event.clientX - bounds.left;
  const y = event.clientY - bounds.top;
  if (x < 0 || x >= bounds.width || y < 0 || y >= bounds.height) return null;
  const column = Math.min(treemapBinColumns - 1,
    Math.floor(x / bounds.width * treemapBinColumns));
  const row = Math.min(treemapBinRows - 1,
    Math.floor(y / bounds.height * treemapBinRows));
  return (treemapBins[row * treemapBinColumns + column] || []).find(leaf =>
    leaf.x0 <= x && x < leaf.x1 && leaf.y0 <= y && y < leaf.y1) || null;
}

function updateTreemapHover(event) {
  const leaf = treemapLeafAt(event);
  if (leaf !== hoveredTreemapLeaf) {
    hoveredTreemapLeaf = leaf;
    if (leaf === null) {
      treemapHover.classList.add("hidden");
      hideTooltip();
      return;
    }
    treemapHover.style.left = `${leaf.x0}px`;
    treemapHover.style.top = `${leaf.y0}px`;
    treemapHover.style.width = `${Math.max(0, leaf.x1 - leaf.x0)}px`;
    treemapHover.style.height = `${Math.max(0, leaf.y1 - leaf.y0)}px`;
    treemapHover.classList.remove("hidden");
    showTooltip(event, {f: leaf.data.function});
  } else if (leaf !== null) {
    moveTooltip(event);
  }
}

function setActiveView(view) {
  activeView = view;
  const showAddress = view === "address";
  addressTab.setAttribute("aria-selected", String(showAddress));
  treemapTab.setAttribute("aria-selected", String(!showAddress));
  addressPanel.classList.toggle("hidden", !showAddress);
  treemapPanel.classList.toggle("hidden", showAddress);
  addressPanel.setAttribute("aria-hidden", String(!showAddress));
  treemapPanel.setAttribute("aria-hidden", String(showAddress));
  hoveredTreemapLeaf = null;
  treemapHover.classList.add("hidden");
  hideTooltip();
  if (showAddress) scheduleMapLayout();
  else scheduleTreemapRender();
}

addressTab.addEventListener("click", () => setActiveView("address"));
treemapTab.addEventListener("click", () => setActiveView("treemap"));
[addressTab, treemapTab].forEach(tab => tab.addEventListener("keydown", event => {
  if (event.key !== "ArrowLeft" && event.key !== "ArrowRight") return;
  event.preventDefault();
  const view = activeView === "address" ? "treemap" : "address";
  setActiveView(view);
  (view === "address" ? addressTab : treemapTab).focus();
}));
treemapCanvas.addEventListener("pointermove", updateTreemapHover);
treemapCanvas.addEventListener("pointerleave", () => {
  hoveredTreemapLeaf = null;
  treemapHover.classList.add("hidden");
  hideTooltip();
});

function labelInView(segment, transform, localX) {
  const start = Math.max(0, localX + segment.x * transform.k);
  const end = Math.min(plotWidth,
    localX + (segment.x + segment.w) * transform.k);
  return {segment, key: segment.key, x: (start + end) / 2,
          width: Math.max(0, end - start)};
}

function renderLabels(transform, localX) {
  labelLayer.attr("transform", `translate(${left},${bitmapY})`);
  renderedLabelScale = transform.k;
  renderedLabelLocalX = localX;
  const [minimum, maximum] = visibleRange(transform, localX);
  const visibleFunctions = [];
  const functionLabelsByRow = Array.from({length: rowCount}, () => []);
  if (transform.k >= 24) {
    for (let row = 0; row < rowCount && visibleFunctions.length < 700; row += 1) {
      visibleSegments(functionRows[row], minimum, maximum).forEach(segment => {
        if (visibleFunctions.length >= 700) return;
        if (!functionIsVisible(segment.f)) return;
        const label = labelInView(segment, transform, localX);
        label.text = fittedLabelText(segment.f.n, label.width);
        if (label.text !== null) {
          visibleFunctions.push(label);
          functionLabelsByRow[row].push(segment);
        }
      });
    }
  }

  const visibleUnits = [];
  for (let row = 0; row < rowCount && visibleUnits.length < 500; row += 1) {
    visibleSegments(outlineRows[row], minimum, maximum).forEach(segment => {
      if (visibleUnits.length >= 500) return;
      if (!unitIsVisible(segment.span.u)) return;
      const hasFunctionLabel = functionLabelsByRow[row].some(f =>
        f.x < segment.x + segment.w && f.x + f.w > segment.x);
      const label = labelInView(segment, transform, localX);
      label.text = fittedUnitLabelText(segment, label.width);
      if (!hasFunctionLabel && label.text !== null) {
        visibleUnits.push(label);
      }
    });
  }

  labelLayer.selectAll("text.unit-label").data(visibleUnits, d => d.key).join(
    enter => enter.append("text").attr("class", "unit-label")
      .attr("text-anchor", "middle").attr("dominant-baseline", "middle"),
    update => update,
    exit => exit.remove())
    .attr("x", d => d.x)
    .attr("y", d => d.segment.row * rowHeight + rowHeight / 2)
    .text(d => d.text);

  labelLayer.selectAll("text.function-label").data(visibleFunctions, d => d.key).join(
    enter => enter.append("text").attr("class", "function-label")
      .attr("text-anchor", "middle").attr("dominant-baseline", "middle"),
    update => update,
    exit => exit.remove())
    .attr("x", d => d.x)
    .attr("y", d => d.segment.row * rowHeight + rowHeight / 2)
    .text(d => d.text);
}

function scheduleRenderedLabelPosition() {
  if (labelPositionScheduled) return;
  labelPositionScheduled = true;
  requestAnimationFrame(() => {
    labelPositionScheduled = false;
    if (!zooming) return;
    drawGestureCanvas();
    updateHoverOutline();
    if (Math.abs(currentTransform.k - renderedLabelScale) < 1e-9) {
      const labelOffset = currentLocalX - renderedLabelLocalX;
      labelLayer.attr("transform",
        `translate(${left + labelOffset},${bitmapY})`);
      return;
    }
    // Scaling only updates positions of labels that already exist. Their
    // expensive visibility scan, data join, and text fitting runs once when
    // the zoom gesture ends.
    const scaleRatio = currentTransform.k / renderedLabelScale;
    labelLayer.attr("transform", `translate(${left},${bitmapY})`)
      .selectAll("text")
      .attr("x", d => currentLocalX +
        (d.x - renderedLabelLocalX) * scaleRatio);
  });
}

const interaction = svg.append("rect").attr("class", "interaction")
  .attr("x", left).attr("y", bitmapY)
  .attr("width", plotWidth).attr("height", bitmapHeight);

function pointerLocation(event) {
  const bounds = svg.node().getBoundingClientRect();
  return [(event.clientX - bounds.left) * width / bounds.width,
          (event.clientY - bounds.top) * totalHeight / bounds.height];
}

function updateHover(event) {
  if (zooming) return;
  const [x, y] = pointerLocation(event);
  const row = Math.floor((y - bitmapY) / rowHeight);
  const untransformedX = (x - left - currentLocalX) / currentTransform.k;
  const segment = row >= 0 && row < rowCount &&
    untransformedX >= 0 && untransformedX <= plotWidth ?
    segmentAt(functionRows[row], untransformedX,
      candidate => functionIsVisible(candidate.f)) : null;
  if (segment !== hoveredSegment) {
    hoveredSegment = segment;
    if (segment === null) hideTooltip();
    else showTooltip(event, segment);
    updateHoverOutline();
  } else if (segment !== null) moveTooltip(event);
}

interaction.on("pointermove", event => {
  lastPointer = {clientX: event.clientX, clientY: event.clientY};
  updateHover(lastPointer);
}).on("pointerleave", () => {
  lastPointer = null;
  hideTooltip();
});

const treeCheckboxes = [];

function makeTreeNode(name) {
  return {name, children: new Map(), leaves: [], unitIds: []};
}

function buildUnitTree() {
  const root = makeTreeNode("");
  data.units.forEach((unit, unitId) => {
    const parts = unit.split("/");
    let node = root;
    parts.slice(0, -1).forEach(part => {
      if (!node.children.has(part)) {
        node.children.set(part, makeTreeNode(part));
      }
      node = node.children.get(part);
    });
    node.leaves.push({name: parts.at(-1), unitId, fullName: unit});
  });

  function collectUnitIds(node) {
    const ids = node.leaves.map(leaf => leaf.unitId);
    node.children.forEach(child => ids.push(...collectUnitIds(child)));
    node.unitIds = ids;
    return ids;
  }
  collectUnitIds(root);
  return root;
}

function createTreeRow(name, unitIds, directory, title) {
  const row = document.createElement("div");
  row.className = "flex min-h-6 items-center whitespace-nowrap";
  let toggle = null;
  if (directory) {
    toggle = document.createElement("button");
    toggle.type = "button";
    toggle.className = "inline-flex h-5 w-5 shrink-0 items-center justify-center rounded text-[var(--muted)] transition-colors hover:bg-[var(--surface-alt)] hover:text-[var(--text)] focus-visible:outline-2 focus-visible:outline-[var(--accent)]";
    toggle.textContent = "▸";
    toggle.setAttribute("aria-expanded", "false");
    toggle.setAttribute("aria-label", `Expand ${name}`);
    row.append(toggle);
  } else {
    const spacer = document.createElement("span");
    spacer.className = "inline-block h-5 w-5 shrink-0";
    row.append(spacer);
  }

  const label = document.createElement("label");
  label.className = "flex min-w-0 cursor-pointer items-center gap-2";
  const checkboxBox = document.createElement("span");
  checkboxBox.className = "relative flex h-4 w-4 shrink-0 items-center justify-center";
  const checkbox = document.createElement("input");
  checkbox.type = "checkbox";
  checkbox.className = "peer h-4 w-4 cursor-pointer appearance-none rounded-[3px] border-2 border-[var(--text)] bg-transparent checked:border-[var(--accent)] checked:bg-[var(--accent)] indeterminate:border-[var(--accent)] indeterminate:bg-[var(--accent)] focus-visible:outline-2 focus-visible:outline-offset-2 focus-visible:outline-[var(--accent)]";
  checkbox.checked = true;
  checkbox.addEventListener("change", () => {
    unitIds.forEach(unitId => { unitVisibility[unitId] = checkbox.checked ? 1 : 0; });
    unitVisibilityChanged();
  });
  const checkmark = document.createElement("span");
  checkmark.setAttribute("aria-hidden", "true");
  checkmark.className = "pointer-events-none absolute hidden text-[11px] font-black leading-none text-[var(--accent-text)] peer-checked:block";
  checkmark.textContent = "✓";
  const mixedMark = document.createElement("span");
  mixedMark.setAttribute("aria-hidden", "true");
  mixedMark.className = "pointer-events-none absolute hidden text-[12px] font-black leading-none text-[var(--accent-text)] peer-indeterminate:block";
  mixedMark.textContent = "−";
  checkboxBox.append(checkbox, checkmark, mixedMark);
  const text = document.createElement("span");
  text.className = `tree-name truncate font-mono${directory ? " font-medium" : ""}`;
  text.textContent = name;
  text.title = title;
  label.append(checkboxBox, text);
  row.append(label);
  treeCheckboxes.push({checkbox, unitIds});
  return {row, toggle};
}

function renderUnitTree(node, container) {
  [...node.children.values()]
    .sort((a, b) => a.name.localeCompare(b.name))
    .forEach(child => {
      const item = document.createElement("li");
      const {row, toggle} = createTreeRow(
        child.name, child.unitIds, true, child.name);
      const children = document.createElement("ul");
      children.className = "ml-3 list-none p-0";
      children.hidden = true;
      toggle.addEventListener("click", () => {
        children.hidden = !children.hidden;
        toggle.textContent = children.hidden ? "▸" : "▾";
        toggle.setAttribute("aria-expanded", String(!children.hidden));
        toggle.setAttribute("aria-label",
          `${children.hidden ? "Expand" : "Collapse"} ${child.name}`);
      });
      item.append(row, children);
      container.append(item);
      renderUnitTree(child, children);
    });

  [...node.leaves]
    .sort((a, b) => a.name.localeCompare(b.name))
    .forEach(leaf => {
      const item = document.createElement("li");
      item.append(createTreeRow(cleanUnitName(leaf.name), [leaf.unitId],
        false, cleanUnitName(leaf.fullName)).row);
      container.append(item);
    });
}

function updateUnitControls() {
  let visibleCount = 0;
  unitVisibility.forEach(visible => { visibleCount += visible; });
  document.getElementById("unit-summary").textContent =
    `${visibleCount}/${data.units.length}`;
  treeCheckboxes.forEach(({checkbox, unitIds}) => {
    let checkedCount = 0;
    unitIds.forEach(unitId => { checkedCount += unitVisibility[unitId]; });
    checkbox.checked = checkedCount === unitIds.length;
    checkbox.indeterminate = checkedCount > 0 && checkedCount < unitIds.length;
  });
}

function unitVisibilityChanged() {
  overviewRows = buildOverviewRows();
  renderLabels(currentTransform, currentLocalX);
  updateUnitControls();
  scheduleDraw();
  scheduleTreemapRender();
  if (lastPointer !== null) updateHover(lastPointer);
}

const unitTreeRoot = document.createElement("ul");
unitTreeRoot.className = "w-full list-none space-y-0.5 p-0";
unitTreeRoot.setAttribute("role", "tree");
document.getElementById("unit-tree").append(unitTreeRoot);
renderUnitTree(buildUnitTree(), unitTreeRoot);
updateUnitControls();
document.getElementById("show-all-units").addEventListener("click", () => {
  unitVisibility.fill(1);
  unitVisibilityChanged();
});
document.getElementById("hide-all-units").addEventListener("click", () => {
  unitVisibility.fill(0);
  unitVisibilityChanged();
});

const zoomStatus = document.getElementById("zoom-status");
const zoom = d3.zoom().scaleExtent([1, 2048])
  .extent([[left, bitmapY], [left + plotWidth, bitmapY + bitmapHeight]])
  .translateExtent([[left, bitmapY],
                    [left + plotWidth, bitmapY + bitmapHeight]])
  .on("start", () => {
    const pendingDraw = drawScheduled;
    cancelScheduledDraw();
    zooming = true;
    hideTooltip();
    if (pendingDraw) drawMap();
    captureGestureCanvas();
    gestureStartScale = currentTransform.k;
    gestureStartLocalX = currentLocalX;
  })
  .on("zoom", event => {
    if (event.sourceEvent && Number.isFinite(event.sourceEvent.clientX)) {
      lastPointer = {clientX: event.sourceEvent.clientX,
                     clientY: event.sourceEvent.clientY};
    }
    currentTransform = event.transform;
    currentLocalX = currentTransform.x + left * (currentTransform.k - 1);
    updateViewportIndicator(currentTransform, currentLocalX);
    scheduleRenderedLabelPosition();
    zoomStatus.textContent = `Zoom ${currentTransform.k < 10 ?
      currentTransform.k.toFixed(1) : Math.round(currentTransform.k)}×`;
  })
  .on("end", () => {
    zooming = false;
    renderLabels(currentTransform, currentLocalX);
    scheduleDraw();
    if (lastPointer !== null) updateHover(lastPointer);
  });
svg.call(zoom).on("dblclick.zoom", null);
document.getElementById("reset").addEventListener("click", () =>
  svg.transition().duration(250).call(zoom.transform, d3.zoomIdentity));

let layoutScheduled = false;
function scheduleMapLayout() {
  if (layoutScheduled) return;
  layoutScheduled = true;
  requestAnimationFrame(() => {
    layoutScheduled = false;
    if (!fitRowsToViewport()) {
      scheduleDraw();
      return;
    }
    [base, svg].forEach(layer =>
      layer.attr("viewBox", `0 0 ${width} ${totalHeight}`));
    positionMapControls();
    bitmapClip.attr("height", bitmapHeight);
    interaction.attr("height", bitmapHeight);
    layoutLabel.text(
      `.text · ${Math.round(rowBytes / 1024)} KiB per row`);
    renderRows();
    xScale = d3.scaleLinear().domain([0, rowBytes]).range([0, plotWidth]);
    rebuildSegmentLayout();
    zoom.extent([[left, bitmapY], [left + plotWidth, bitmapY + bitmapHeight]])
      .translateExtent([[left, bitmapY],
                        [left + plotWidth, bitmapY + bitmapHeight]]);
    svg.call(zoom.transform, d3.zoomIdentity);
    scheduleDraw();
  });
}

new ResizeObserver(scheduleMapLayout).observe(mapStack.node());
new ResizeObserver(scheduleTreemapRender).observe(treemapPanel);
colorScheme.addEventListener("change", () => {
  document.documentElement.dataset.theme = colorScheme.matches ? "dark" : "light";
  scheduleDraw();
  scheduleTreemapRender();
});
scheduleDraw();
renderLabels(d3.zoomIdentity, 0);
updateViewportIndicator(d3.zoomIdentity, 0);
</script>
</body>
</html>
"""


def render_html(payload):
    data = json.dumps(payload, separators=(",", ":"), ensure_ascii=True)
    return HTML_TEMPLATE.replace("/*__DATA__*/", data.replace("</", "<\\/"))


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "report",
        nargs="?",
        default="report.json",
        help="objdiff report (default: report.json)",
    )
    parser.add_argument(
        "--binary",
        default="res/libTTapp.so",
        help="original ELF binary (default: res/libTTapp.so)",
    )
    parser.add_argument(
        "-o",
        "--output",
        default="pages/binary_match_map.html",
        help="output HTML (default: pages/binary_match_map.html)",
    )
    parser.add_argument(
        "--bytes-per-cell",
        type=int,
        default=512,
        help="address bytes represented by a cell (default: 512)",
    )
    parser.add_argument(
        "--columns",
        type=int,
        default=256,
        help="fallback cells per row before viewport sizing " "(default: 256)",
    )
    args = parser.parse_args()

    if args.bytes_per_cell < 1 or args.columns < 1:
        parser.error("--bytes-per-cell and --columns must be at least 1")

    try:
        with open(args.report) as report_file:
            report = json.load(report_file)
        sections, symbols = read_elf32(args.binary)
    except (OSError, ValueError, json.JSONDecodeError, struct.error) as error:
        print(f"error: {error}", file=sys.stderr)
        return 1

    text_section = next(
        (section for section in sections if section["name"] == ".text"), None
    )
    if text_section is None:
        print(f"error: no .text section in {args.binary}", file=sys.stderr)
        return 1

    functions = [
        symbol
        for symbol in symbols
        if symbol["type"] == STT_FUNC
        and symbol["section_index"] == text_section["index"]
    ]
    scores, units, names = report_function_details(report)
    unit_spans = build_unit_spans(text_section, functions, units)
    row_bytes = args.columns * args.bytes_per_cell
    payload = build_payload(
        sections,
        text_section,
        functions,
        scores,
        units,
        names,
        unit_spans,
        row_bytes,
        args.bytes_per_cell,
    )
    try:
        with open(args.output, "w", encoding="utf-8") as output_file:
            output_file.write(render_html(payload))
    except OSError as error:
        print(f"error: could not write {args.output}: {error}", file=sys.stderr)
        return 1

    duplicate_names = Counter(function["name"] for function in functions)
    ambiguous_functions = sum(
        1 for function in functions if duplicate_names[function["name"]] > 1
    )
    scored_functions = sum(
        1
        for function in functions
        if duplicate_names[function["name"]] == 1 and function["name"] in scores
    )
    print(
        f"Mapped {scored_functions} scored and {ambiguous_functions} ambiguous "
        f".text symbols with {len(unit_spans)} unit spans across "
        f"responsive viewport-fitted rows to {args.output}"
    )
    return 0


if __name__ == "__main__":
    sys.exit(main())
