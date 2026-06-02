const fs = require('fs');
const path = require('path');

describe("WASM loader should implement integrity verification for fetched resources", () => {
  const startJsPath = path.join(__dirname, 'backends/system/wasm/js-sources/start.js');
  
  // Test payloads representing different attack scenarios on the fetch URL
  const payloads = [
    { url: "./ShaderTest.wasm", description: "original unverified fetch" },
    { url: "../../../malicious.wasm", description: "path traversal attack" },
    { url: "https://evil.com/ShaderTest.wasm", description: "external domain injection" },
  ];

  test("source code must include integrity verification for WASM fetch", () => {
    const sourceCode = fs.readFileSync(startJsPath, 'utf8');
    
    // Security property: WASM fetches MUST have integrity verification
    const hasIntegrityCheck = 
      sourceCode.includes('integrity') ||
      sourceCode.includes('SRI') ||
      sourceCode.includes('sha256') ||
      sourceCode.includes('sha384') ||
      sourceCode.includes('sha512') ||
      sourceCode.includes('verify') ||
      sourceCode.includes('checksum');
    
    const hasCSPRestriction = 
      sourceCode.includes('Content-Security-Policy') ||
      sourceCode.includes('script-src') ||
      sourceCode.includes('wasm-unsafe-eval');

    const hasSignatureVerification =
      sourceCode.includes('signature') ||
      sourceCode.includes('crypto.subtle');

    const hasAnyProtection = hasIntegrityCheck || hasCSPRestriction || hasSignatureVerification;
    
    expect(hasAnyProtection).toBe(true);
  });

  test.each(payloads)("fetch of $description should be protected", ({ url }) => {
    const sourceCode = fs.readFileSync(startJsPath, 'utf8');
    
    // If the URL pattern exists in code, it must be accompanied by integrity checks
    if (sourceCode.includes(url) || sourceCode.includes('fetch(')) {
      const fetchPattern = /fetch\s*\([^)]+\)/g;
      const fetches = sourceCode.match(fetchPattern) || [];
      
      fetches.forEach(fetchCall => {
        // Security invariant: fetch calls for WASM should not be bare/unverified
        const isBareWasmFetch = fetchCall.includes('.wasm') && 
          !sourceCode.includes('integrity');
        
        expect(isBareWasmFetch).toBe(false);
      });
    }
  });
});