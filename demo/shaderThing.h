std::ifstream f(filepath);
if (!f.is_open()) { throw std::runtime_error("File '" + filepath + "' could not be opened."); }

std::stringstream buffer;
buffer << f.rdbuf();
std::string source = buffer.str();
std::string s = source;
std::string splitStr[4];

// determine the indices using RegEx
size_t indices[4];
size_t offset = 0;
std::smatch match;

for (int i = 0; std::regex_search(s, match, std::regex("(#type)( )+([a-zA-Z]+)")); i+=2) {
    indices[i] = match.position() + offset;
    indices[i + 1] = indices[i] + match.length();
    s = match.suffix();
    offset = source.length() - s.length();
}

splitStr[0] = source.substr(indices[0], indices[1]); // type of the first part
splitStr[1] = source.substr(indices[1] + 1, indices[2]); // store the first part of the shader data 
splitStr[2] = source.substr(indices[2], indices[3]); // type of the second part
splitStr[3] = source.substr(indices[3] + 1); // store the second part of the shader data

// first part
if (splitStr[0] == "vertex") { vertexSource = splitStr[1].c_str(); vertexSize[0] = splitStr[1].length(); }
else if (splitStr[0] == "fragment") { fragmentSource = splitStr[1].c_str(); fragmentSize[0] = splitStr[1].length(); }
else { throw std::runtime_error("Unexpected token '" + splitStr[0] + "'"); }

// second part
if (splitStr[2] == "vertex") { vertexSource = splitStr[3].c_str(); vertexSize[0] = splitStr[3].length(); }
else if (splitStr[2] == "fragment") { fragmentSource = splitStr[3].c_str(); fragmentSize[0] = splitStr[3].length(); }
else { throw std::runtime_error("Unexpected token '" + splitStr[2] + "'"); }