#pragma once

#include "../Core/Core.h"

#include <unordered_map>

namespace Pengine 
{

	struct PENGINE_API ShaderProgramSource
	{
		std::string m_VertexSource;
		std::string m_FragmentSource;
	};

	class PENGINE_API Shader
	{
	private:

		static std::unordered_map<std::string, Shader*> s_Shaders;
		static const char** s_NamesOfShaders;

		std::unordered_map<std::string, int> m_UniformLocationCache;
		uint32_t m_RendererID;

		std::string m_Name;
		std::string m_FilePath;

		int GetUniformLocation(const std::string& name);
		
		Shader() {}
		Shader(const std::string& name,const std::string& filepath);
		~Shader();
	public:

		static const char** GetListOfShaders();
		
		static Shader* Create(const std::string& name, const std::string& path);
		
		static Shader* Get(const std::string& name, bool showErrors = true);
		
		void Bind() const;
		
		void UnBind() const;
		
		void Reload();

		std::string GetName() const { return m_Name; }
		
		std::string GetFilePath() const { return m_FilePath; }

		ShaderProgramSource Parseshader(const std::string& filepath);
		
		uint32_t CreateShader(const std::string& vertexShader, const std::string& FragmentShader);
		
		uint32_t CompileShader(uint32_t type, const std::string& source);

		void SetUniform1i(const std::string& name, int value);
		void SetUniform1iv(const std::string& name, int* value, size_t count = 32);

		void SetUniform4f(const std::string& name, float v0, float v1, float v2, float fv3);
		void SetUniform3f(const std::string& name, float v0, float v1, float v2);
		void SetUniform2f(const std::string& name, float v0, float v1);

		void SetUniform4fv(const std::string& name,const glm::vec4& vec4);
		void SetUniform3fv(const std::string& name,const glm::vec3& vec3);
		void SetUniform2fv(const std::string& name,const glm::vec2& vec);

		void SetUniform1f(const std::string& name, float v0);

		void SetUniformMat4f(const std::string& name,const glm::mat4& matrix);

		friend class Editor;
	};

}