// An index entry gives a search result without a text snippet: Sphinx's summary is empty for its
// anchor and appending it throws. Keep such a result only for a page nothing else found, labelled.
// Sphinx loads searchtools.js after this file and starts searching once the document is parsed.
document.addEventListener("DOMContentLoaded", () => {
  if (typeof Search === "undefined") {
    return;
  }
  const performSearch = Search._performSearch;
  Search._performSearch = (...args) => {
    const results = performSearch(...args);
    const kind = (result) => result[6];
    const page = (result) => result[0];
    const found = new Set(results.filter((r) => kind(r) !== "index").map(page));
    return results.filter((r) => {
      if (kind(r) !== "index") {
        return true;
      }
      if (found.has(page(r))) {
        return false;
      }
      found.add(page(r));
      r[3] = "index entry";
      return true;
    });
  };
});
