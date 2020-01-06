module.exports = {
  collectCoverageFrom: ['src/**/*.{js,jsx,ts,tsx,mjs}'],
  moduleFileExtensions: ['js', 'jsx', 'ts', 'tsx', 'json', 'vue'],
  roots: ['<rootDir>/src'],
  testPathIgnorePatterns: ['/node_modules/', '/lib/'],
  testRegex: '(/(test|__tests__)/.*(\\.|/)(test|spec))\\.[j|t]sx?$',
  testURL: 'http://localhost',
  transform: {
    '^.+\\.(js|ts)x?$': 'babel-jest',
  },
  transformIgnorePatterns: ['node_modules/(?!(@glennsl/bs-jest|bs-platform)/)'],
}
