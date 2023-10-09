const IndexPage = () => (
  <div
    style={{
      backgroundColor: "#fff",
      width: "100%",
      height: "100%",
      margin: "0px",
      padding: "0px",
    }}
  >
    <h1 style={{ color: "#000", textAlign: "center" }}>
      GATE OPENER API v1.0.0
    </h1>
    <p style={{ color: "#000", textAlign: "center" }}>
      generate a 64 chars string from{" "}
      <a
        href="https://miniwebtool.com/pl/random-string-generator/"
        target="_blank"
      >
        there
      </a>{" "}
      and go to api/[string]
    </p>
  </div>
);

export default IndexPage;
